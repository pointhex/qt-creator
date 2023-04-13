// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "clangformatconfigwidget.h"

#include "clangformatchecks.h"
#include "clangformatconstants.h"
#include "clangformatfile.h"
#include "clangformatindenter.h"
#include "clangformattr.h"
#include "clangformatutils.h"

// the file was generated by scripts/generateClangFormatChecksLayout.py

#include <coreplugin/icore.h>

#include <cppeditor/cppcodestylepreferences.h>
#include <cppeditor/cppcodestylesettings.h>
#include <cppeditor/cppcodestylesnippets.h>
#include <cppeditor/cpphighlighter.h>
#include <cppeditor/cpptoolssettings.h>

#include <projectexplorer/editorconfiguration.h>
#include <projectexplorer/project.h>

#include <texteditor/displaysettings.h>
#include <texteditor/icodestylepreferences.h>
#include <texteditor/snippets/snippeteditor.h>
#include <texteditor/textdocument.h>
#include <texteditor/texteditorsettings.h>

#include <utils/guard.h>
#include <utils/layoutbuilder.h>
#include <utils/qtcassert.h>

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QSharedPointer>
#include <QVBoxLayout>
#include <QWeakPointer>
#include <QWidget>

#include <clang/Format/Format.h>

#include <sstream>

using namespace ProjectExplorer;
using namespace Utils;

namespace ClangFormat {

class ClangFormatConfigWidget::Private
{
public:
    ProjectExplorer::Project *project = nullptr;
    QWidget *checksWidget = nullptr;
    QScrollArea *checksScrollArea = nullptr;
    TextEditor::SnippetEditorWidget *preview = nullptr;
    std::unique_ptr<ClangFormatFile> config;
    clang::format::FormatStyle style;
    Utils::Guard ignoreChanges;
    QLabel *fallbackConfig;
};

bool ClangFormatConfigWidget::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::Wheel && qobject_cast<QComboBox *>(object)) {
        event->ignore();
        return true;
    }
    return QWidget::eventFilter(object, event);
}

ClangFormatConfigWidget::ClangFormatConfigWidget(TextEditor::ICodeStylePreferences *codeStyle,
                                                 ProjectExplorer::Project *project,
                                                 QWidget *parent)
    : CppCodeStyleWidget(parent), d(new Private)
{
    d->project = project;
    d->config = std::make_unique<ClangFormatFile>(filePathToCurrentSettings(codeStyle->currentPreferences()));

    resize(489, 305);
    d->fallbackConfig = new QLabel(Tr::tr("Clang-Format Style"));
    d->checksScrollArea = new QScrollArea();
    d->checksWidget = new ClangFormatChecks();

    d->checksScrollArea->setWidget(d->checksWidget);
    d->checksScrollArea->setWidgetResizable(true);
    d->checksWidget->setEnabled(!codeStyle->isReadOnly()
                                && !codeStyle->isTemporarilyReadOnly());

    FilePath fileName;
    if (d->project)
        fileName = d->project->projectFilePath().pathAppended("snippet.cpp");
    else
        fileName = Core::ICore::userResourcePath("snippet.cpp");

    d->preview = new TextEditor::SnippetEditorWidget(this);
    TextEditor::DisplaySettings displaySettings = d->preview->displaySettings();
    displaySettings.m_visualizeWhitespace = true;
    d->preview->setDisplaySettings(displaySettings);
    d->preview->setPlainText(QLatin1String(CppEditor::Constants::DEFAULT_CODE_STYLE_SNIPPETS[0]));
    d->preview->textDocument()->setIndenter(new ClangFormatIndenter(d->preview->document()));
    d->preview->textDocument()->setFontSettings(TextEditor::TextEditorSettings::fontSettings());
    d->preview->textDocument()->setSyntaxHighlighter(new CppEditor::CppHighlighter);
    d->preview->textDocument()->indenter()->setFileName(fileName);

    using namespace Layouting;

    Column {
        d->fallbackConfig,
        Row { d->checksScrollArea, d->preview },
    }.attachTo(this);

    connect(codeStyle, &TextEditor::ICodeStylePreferences::currentPreferencesChanged,
            this, &ClangFormatConfigWidget::slotCodeStyleChanged);

    slotCodeStyleChanged(codeStyle->currentPreferences());

    showOrHideWidgets();
    fillTable();
    updatePreview();

    connectChecks();
}

ClangFormatConfigWidget::~ClangFormatConfigWidget()
{
    delete d;
}

void ClangFormatConfigWidget::slotCodeStyleChanged(
    TextEditor::ICodeStylePreferences *codeStyle)
{
    if (!codeStyle)
        return;
    d->config.reset(new ClangFormatFile(filePathToCurrentSettings(codeStyle)));
    d->config->setIsReadOnly(codeStyle->isReadOnly());
    d->style = d->config->style();

    d->checksWidget->setEnabled(!codeStyle->isReadOnly()
                                && !codeStyle->isTemporarilyReadOnly());

    fillTable();
    updatePreview();
}

void ClangFormatConfigWidget::connectChecks()
{
    auto doSaveChanges = [this](QObject *sender) {
        if (!d->ignoreChanges.isLocked())
            saveChanges(sender);
    };

    for (QObject *child : d->checksWidget->children()) {
        auto comboBox = qobject_cast<QComboBox *>(child);
        if (comboBox != nullptr) {
            connect(comboBox, &QComboBox::currentIndexChanged,
                    this, std::bind(doSaveChanges, comboBox));
            comboBox->installEventFilter(this);
            continue;
        }

        const auto button = qobject_cast<QPushButton *>(child);
        if (button != nullptr)
            connect(button, &QPushButton::clicked, this, std::bind(doSaveChanges, button));
    }
}

static clang::format::FormatStyle constructStyle(const QByteArray &baseStyle = QByteArray())
{
    if (!baseStyle.isEmpty()) {
        // Try to get the style for this base style.
        llvm::Expected<clang::format::FormatStyle> style
            = clang::format::getStyle(baseStyle.toStdString(), "dummy.cpp", baseStyle.toStdString());
        if (style)
            return *style;

        handleAllErrors(style.takeError(), [](const llvm::ErrorInfoBase &) {
            // do nothing
        });
        // Fallthrough to the default style.
    }
    return qtcStyle();
}

Utils::FilePath ClangFormatConfigWidget::globalPath()
{
    return Core::ICore::userResourcePath();
}

Utils::FilePath ClangFormatConfigWidget::projectPath()
{
    if (d->project)
        return globalPath().pathAppended("clang-format/" + projectUniqueId(d->project));

    return Utils::FilePath();
}

void ClangFormatConfigWidget::createStyleFileIfNeeded(bool isGlobal)
{
    const FilePath path = isGlobal ? globalPath() : projectPath();
    const FilePath configFile = path / Constants::SETTINGS_FILE_NAME;

    if (configFile.exists())
        return;

    QDir().mkpath(path.toString());
    if (!isGlobal) {
        FilePath possibleProjectConfig = d->project->rootProjectDirectory()
                                         / Constants::SETTINGS_FILE_NAME;
        if (possibleProjectConfig.exists()) {
            // Just copy th .clang-format if current project has one.
            possibleProjectConfig.copyFile(configFile);
            return;
        }
    }

    std::fstream newStyleFile(configFile.toString().toStdString(), std::fstream::out);
    if (newStyleFile.is_open()) {
        newStyleFile << clang::format::configurationAsText(constructStyle());
        newStyleFile.close();
    }
}

void ClangFormatConfigWidget::showOrHideWidgets()
{
    auto verticalLayout = qobject_cast<QVBoxLayout *>(layout());
    QTC_ASSERT(verticalLayout, return);

    QLayoutItem *lastItem = verticalLayout->itemAt(verticalLayout->count() - 1);
    if (lastItem->spacerItem())
        verticalLayout->removeItem(lastItem);

    createStyleFileIfNeeded(!d->project);
    d->fallbackConfig->show();
    d->checksScrollArea->show();
    d->preview->show();
}

void ClangFormatConfigWidget::updatePreview()
{
    QTextCursor cursor(d->preview->document());
    cursor.setPosition(0);
    cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    d->preview->textDocument()->autoIndent(cursor);
}

static inline void ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) { return !std::isspace(ch); }));
}

static inline void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) { return !std::isspace(ch); }).base(),
            s.end());
}

static inline void trim(std::string &s)
{
    ltrim(s);
    rtrim(s);
}

static void fillPlainText(QPlainTextEdit *plainText, const std::string &text, size_t index)
{
    if (index == std::string::npos) {
        plainText->setPlainText("");
        return;
    }
    size_t valueStart = text.find('\n', index + 1);
    size_t valueEnd;
    std::string value;
    QTC_ASSERT(valueStart != std::string::npos, return;);
    do {
        valueEnd = text.find('\n', valueStart + 1);
        if (valueEnd == std::string::npos)
            break;
        // Skip also 2 spaces - start with valueStart + 1 + 2.
        std::string line = text.substr(valueStart + 3, valueEnd - valueStart - 3);
        rtrim(line);
        value += value.empty() ? line : '\n' + line;
        valueStart = valueEnd;
    } while (valueEnd < text.size() - 1 && text.at(valueEnd + 1) == ' ');
    plainText->setPlainText(QString::fromStdString(value));
}

static void fillComboBoxOrLineEdit(QObject *object, const std::string &text, size_t index)
{
    auto *comboBox = qobject_cast<QComboBox *>(object);
    auto *lineEdit = qobject_cast<QLineEdit *>(object);
    if (index == std::string::npos) {
        if (comboBox)
            comboBox->setCurrentIndex(0);
        else
            lineEdit->setText("");
        return;
    }

    const size_t valueStart = text.find(':', index + 1);
    QTC_ASSERT(valueStart != std::string::npos, return;);
    const size_t valueEnd = text.find('\n', valueStart + 1);
    QTC_ASSERT(valueEnd != std::string::npos, return;);
    std::string value = text.substr(valueStart + 1, valueEnd - valueStart - 1);
    trim(value);

    if (comboBox) {
        if (comboBox->findText(QString::fromStdString(value)) == -1) {
            comboBox->setCurrentIndex(0);
            return;
        }
        comboBox->setCurrentText(QString::fromStdString(value));
        return;
    }

    lineEdit->setText(QString::fromStdString(value));
}

std::string ClangFormatConfigWidget::readFile(const QString &path)
{
    const std::string defaultStyle = clang::format::configurationAsText(qtcStyle());

    QFile file(path);
    if (!file.open(QFile::ReadOnly))
        return defaultStyle;

    const std::string content = file.readAll().toStdString();
    file.close();

    clang::format::FormatStyle style;
    style.Language = clang::format::FormatStyle::LK_Cpp;
    const std::error_code error = clang::format::parseConfiguration(content, &style);
    QTC_ASSERT(error.value() == static_cast<int>(clang::format::ParseError::Success),
               return defaultStyle);

    addQtcStatementMacros(style);
    std::string settings = clang::format::configurationAsText(style);

    // Needed workaround because parseConfiguration remove BasedOnStyle field
    // ToDo: standardize this behavior for future
    const size_t index = content.find("BasedOnStyle");
    if (index != std::string::npos) {
        const size_t size = content.find("\n", index) - index;
        const size_t insert_index = settings.find("\n");
        settings.insert(insert_index, "\n" + content.substr(index, size));
    }

    return settings;
}

void ClangFormatConfigWidget::fillTable()
{
    Utils::GuardLocker locker(d->ignoreChanges);

    const std::string configText = readFile(d->config->filePath().path());

    for (QObject *child : d->checksWidget->children()) {
        if (!qobject_cast<QComboBox *>(child) && !qobject_cast<QLineEdit *>(child)
            && !qobject_cast<QPlainTextEdit *>(child)) {
            continue;
        }

        size_t index = configText.find('\n' + child->objectName().toStdString());
        if (index == std::string::npos)
            index = configText.find("\n  " + child->objectName().toStdString());

        if (qobject_cast<QPlainTextEdit *>(child))
            fillPlainText(qobject_cast<QPlainTextEdit *>(child), configText, index);
        else
            fillComboBoxOrLineEdit(child, configText, index);
    }
}

void ClangFormatConfigWidget::saveChanges(QObject *sender)
{
    if (sender->objectName() == "BasedOnStyle") {
        const auto *basedOnStyle = d->checksWidget->findChild<QComboBox *>("BasedOnStyle");
        d->config->setBasedOnStyle(basedOnStyle->currentText());
    } else {
        QList<ClangFormatFile::Field> fields;

        for (QObject *child : d->checksWidget->children()) {
            if (child->objectName() == "BasedOnStyle")
                continue;
            auto *label = qobject_cast<QLabel *>(child);
            if (!label)
                continue;

            QWidget *valueWidget = d->checksWidget->findChild<QWidget *>(label->text().trimmed());
            if (!valueWidget) {
                // Currently BraceWrapping only.
                fields.append({label->text(), ""});
                continue;
            }

            if (!qobject_cast<QComboBox *>(valueWidget) && !qobject_cast<QLineEdit *>(valueWidget)
                && !qobject_cast<QPlainTextEdit *>(valueWidget)) {
                continue;
            }

            auto *plainText = qobject_cast<QPlainTextEdit *>(valueWidget);
            if (plainText) {
                if (plainText->toPlainText().trimmed().isEmpty())
                    continue;


                std::stringstream content;
                QStringList list = plainText->toPlainText().split('\n');
                for (const QString &line : list)
                    content << "\n  " << line.toStdString();

                fields.append({label->text(), QString::fromStdString(content.str())});
            } else {
                QString text;
                if (auto *comboBox = qobject_cast<QComboBox *>(valueWidget)) {
                    text = comboBox->currentText();
                } else {
                    auto *lineEdit = qobject_cast<QLineEdit *>(valueWidget);
                    QTC_ASSERT(lineEdit, continue;);
                    text = lineEdit->text();
                }

                if (!text.isEmpty() && text != "Default")
                    fields.append({label->text(), text});
            }
        }
        d->config->changeFields(fields);
    }

    fillTable();
    updatePreview();
    synchronize();
}

void ClangFormatConfigWidget::setCodeStyleSettings(const CppEditor::CppCodeStyleSettings &settings)
{
    d->config->fromCppCodeStyleSettings(settings);

    fillTable();
    updatePreview();
}

void ClangFormatConfigWidget::setTabSettings(const TextEditor::TabSettings &settings)
{
    d->config->fromTabSettings(settings);

    fillTable();
    updatePreview();
}

void ClangFormatConfigWidget::synchronize()
{
    emit codeStyleSettingsChanged(d->config->toCppCodeStyleSettings(d->project));
    emit tabSettingsChanged(d->config->toTabSettings(d->project));
}

void ClangFormatConfigWidget::apply()
{
    if (!d->checksWidget->isVisible() && !d->checksWidget->isEnabled())
        return;

    d->style = d->config->style();
}

void ClangFormatConfigWidget::finish()
{
    if (!d->checksWidget->isVisible() && !d->checksWidget->isEnabled())
        return;

    d->config->setStyle(d->style);
}

} // namespace ClangFormat
