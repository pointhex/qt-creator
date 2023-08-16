// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "valgrindrunner.h"

#include "valgrindtr.h"
#include "xmlprotocol/parser.h"

#include <projectexplorer/runcontrol.h>

#include <utils/hostosinfo.h>
#include <utils/process.h>
#include <utils/qtcassert.h>

#include <QEventLoop>
#include <QTcpServer>
#include <QTcpSocket>

using namespace ProjectExplorer;
using namespace Utils;
using namespace Valgrind::XmlProtocol;

namespace Valgrind {

class ValgrindRunner::Private : public QObject
{
public:
    Private(ValgrindRunner *owner) : q(owner) {
        connect(&m_process, &Process::started, this, [this] {
            emit q->valgrindStarted(m_process.processId());
        });
        connect(&m_process, &Process::done, this, [this] {
            if (m_process.result() != ProcessResult::FinishedWithSuccess)
                emit q->processErrorReceived(m_process.errorString(), m_process.error());
            emit q->finished();
        });
        connect(&m_process, &Process::readyReadStandardOutput, this, [this] {
            emit q->appendMessage(m_process.readAllStandardOutput(), StdOutFormat);
        });
        connect(&m_process, &Process::readyReadStandardError, this, [this] {
            emit q->appendMessage(m_process.readAllStandardError(), StdErrFormat);
        });

        connect(&m_xmlServer, &QTcpServer::newConnection, this, [this] {
            QTcpSocket *socket = m_xmlServer.nextPendingConnection();
            QTC_ASSERT(socket, return);
            m_xmlServer.close();
            m_parser.setSocket(socket);
            m_parser.start();
        });
        connect(&m_logServer, &QTcpServer::newConnection, this, [this] {
            QTcpSocket *socket = m_logServer.nextPendingConnection();
            QTC_ASSERT(socket, return);
            connect(socket, &QIODevice::readyRead, this, [this, socket] {
                emit q->logMessageReceived(socket->readAll());
            });
            m_logServer.close();
        });

        connect(&m_parser, &Parser::status, q, &ValgrindRunner::status);
        connect(&m_parser, &Parser::error, q, &ValgrindRunner::error);
        connect(&m_parser, &Parser::done, this, [this](bool success, const QString &err) {
            if (!success)
                emit q->internalError(err);
        });
    }

    bool startServers();
    bool run();

    ValgrindRunner *q = nullptr;

    CommandLine m_valgrindCommand;
    Runnable m_debuggee;
    QProcess::ProcessChannelMode m_channelMode = QProcess::SeparateChannels;
    QHostAddress m_localServerAddress;
    bool m_useTerminal = false;

    Process m_process;
    QTcpServer m_xmlServer;
    QTcpServer m_logServer;
    Parser m_parser;
};

bool ValgrindRunner::Private::startServers()
{
    const bool xmlOK = m_xmlServer.listen(m_localServerAddress);
    const QString ip = m_localServerAddress.toString();
    if (!xmlOK) {
        emit q->processErrorReceived(Tr::tr("XmlServer on %1:").arg(ip) + ' '
                                     + m_xmlServer.errorString(), QProcess::FailedToStart );
        return false;
    }
    m_xmlServer.setMaxPendingConnections(1);
    const bool logOK = m_logServer.listen(m_localServerAddress);
    if (!logOK) {
        emit q->processErrorReceived(Tr::tr("LogServer on %1:").arg(ip) + ' '
                                     + m_logServer.errorString(), QProcess::FailedToStart );
        return false;
    }
    m_logServer.setMaxPendingConnections(1);
    return true;
}

bool ValgrindRunner::Private::run()
{
    CommandLine cmd;
    cmd.setExecutable(m_valgrindCommand.executable());

    if (!m_localServerAddress.isNull()) {
        if (!startServers())
            return false;

        cmd.addArg("--child-silent-after-fork=yes");

        // Workaround for valgrind bug when running vgdb with xml output
        // https://bugs.kde.org/show_bug.cgi?id=343902
        bool enableXml = true;

        auto handleSocketParameter = [&enableXml, &cmd](const QString &prefix, const QTcpServer &tcpServer)
        {
            QHostAddress serverAddress = tcpServer.serverAddress();
            if (serverAddress.protocol() != QAbstractSocket::IPv4Protocol) {
                // Report will end up in the Application Output pane, i.e. not have
                // clickable items, but that's better than nothing.
                qWarning("Need IPv4 for valgrind");
                enableXml = false;
            } else {
                cmd.addArg(QString("%1=%2:%3").arg(prefix).arg(serverAddress.toString())
                           .arg(tcpServer.serverPort()));
            }
        };

        handleSocketParameter("--xml-socket", m_xmlServer);
        handleSocketParameter("--log-socket", m_logServer);

        if (enableXml)
            cmd.addArg("--xml=yes");
    }
    cmd.addArgs(m_valgrindCommand.arguments(), CommandLine::Raw);

    // consider appending our options last so they override any interfering user-supplied options
    // -q as suggested by valgrind manual

    if (cmd.executable().osType() == OsTypeMac) {
        // May be slower to start but without it we get no filenames for symbols.
        cmd.addArg("--dsymutil=yes");
    }

    cmd.addCommandLineAsArgs(m_debuggee.command);

    emit q->appendMessage(cmd.toUserOutput(), NormalMessageFormat);

    m_process.setCommand(cmd);
    m_process.setWorkingDirectory(m_debuggee.workingDirectory);
    m_process.setEnvironment(m_debuggee.environment);
    m_process.setProcessChannelMode(m_channelMode);
    m_process.setTerminalMode(m_useTerminal ? TerminalMode::Run : TerminalMode::Off);
    m_process.start();
    return true;
}

ValgrindRunner::ValgrindRunner(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{}

ValgrindRunner::~ValgrindRunner()
{
    if (d->m_process.isRunning()) {
        // make sure we don't delete the thread while it's still running
        waitForFinished();
    }
    if (d->m_parser.isRunning()) {
        // make sure we don't delete the thread while it's still running
        waitForFinished();
    }
    delete d;
    d = nullptr;
}

void ValgrindRunner::setValgrindCommand(const CommandLine &command)
{
    d->m_valgrindCommand = command;
}

void ValgrindRunner::setDebuggee(const Runnable &debuggee)
{
    d->m_debuggee = debuggee;
}

void ValgrindRunner::setProcessChannelMode(QProcess::ProcessChannelMode mode)
{
    d->m_channelMode = mode;
}

void ValgrindRunner::setLocalServerAddress(const QHostAddress &localServerAddress)
{
    d->m_localServerAddress = localServerAddress;
}

void ValgrindRunner::setUseTerminal(bool on)
{
    d->m_useTerminal = on;
}

void ValgrindRunner::waitForFinished() const
{
    if (d->m_process.state() == QProcess::NotRunning)
        return;

    QEventLoop loop;
    connect(this, &ValgrindRunner::finished, &loop, &QEventLoop::quit);
    loop.exec();
}

QString ValgrindRunner::errorString() const
{
    return d->m_process.errorString();
}

bool ValgrindRunner::start()
{
    return d->run();
}

void ValgrindRunner::stop()
{
    d->m_process.stop();
}

} // namespace Valgrind
