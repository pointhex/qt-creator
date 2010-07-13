#include "%PluginName:l%.%CppHeaderSuffix%"

#include <QtPlugin>

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>

#include <QtPlugin>
#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>

namespace {
    const char * const ACTION_ID = "%PluginName%.Action";
    const char * const MENU_ID = "%PluginName%.Menu";
}

using namespace %PluginName%::Internal;

%PluginName%Impl::%PluginName%Impl()
{
    // Create your members
}

%PluginName%Impl::~%PluginName%Impl()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members
}

bool %PluginName%Impl::initialize(const QStringList &arguments, QString *errorString)
{
    // Register objects in the plugin manager's object pool
    // Load settings
    // Add actions to menus
    // connect to other plugins' signals
    // "In the initialize method, a plugin can be sure that the plugins it
    //  depends on have initialized their members."

    Q_UNUSED(arguments)
    Q_UNUSED(errorString)
    Core::ActionManager *am = Core::ICore::instance()->actionManager();

    QAction *action = new QAction(tr("%PluginName% action"), this);
    Core::Command *cmd = am->registerAction(action, QLatin1String(ACTION_ID),
                         Core::Context(Core::Constants::C_GLOBAL));
    cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Alt+Meta+A")));
    connect(action, SIGNAL(triggered()), this, SLOT(triggerAction()));

    Core::ActionContainer *menu = am->createMenu(MENU_ID);
    menu->menu()->setTitle(tr("%PluginName%"));
    menu->addAction(cmd);
    am->actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);

    return true;
}

void %PluginName%Impl::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // "In the extensionsInitialized method, a plugin can be sure that all
    //  plugins that depend on it are completely initialized."
}

ExtensionSystem::IPlugin::ShutdownFlag %PluginName%Impl::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

void %PluginName%Impl::triggerAction()
{
    QMessageBox::information(Core::ICore::instance()->mainWindow(),
                             tr("Action triggered"),
                             tr("This is an action from %PluginName%."));
}

Q_EXPORT_PLUGIN2(%PluginName%, %PluginName%Impl)
