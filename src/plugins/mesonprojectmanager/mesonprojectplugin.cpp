// Copyright (C) 2020 Alexis Jeandet.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "mesonactionsmanager.h"
#include "mesonbuildconfiguration.h"
#include "mesonbuildsystem.h"
#include "mesonproject.h"
#include "mesonrunconfiguration.h"
#include "ninjabuildstep.h"
#include "toolssettingsaccessor.h"
#include "toolssettingspage.h"

#include <debugger/debuggerruncontrol.h>

#include <extensionsystem/iplugin.h>

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/projectmanager.h>
#include <projectexplorer/runcontrol.h>

#include <utils/fsengine/fileiconprovider.h>

using namespace Debugger;
using namespace ProjectExplorer;
using namespace Utils;

namespace MesonProjectManager::Internal {

class MesonProjectPluginPrivate
{
public:
    MesonRunConfigurationFactory m_runConfigurationFactory;
    MesonActionsManager m_actions;
    MachineFileManager m_machineFilesManager;
    SimpleTargetRunnerFactory m_mesonRunWorkerFactory{{m_runConfigurationFactory.runConfigurationId()}};
    SimpleDebugRunnerFactory m_mesonDebugRunWorkerFactory{{m_runConfigurationFactory.runConfigurationId()}};
};

class MesonProjectPlugin final : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "MesonProjectManager.json")

public:
    ~MesonProjectPlugin() final
    {
        delete d;
    }

private:
    void initialize() final
    {
        d = new MesonProjectPluginPrivate;

        setupToolsSettingsPage();
        setupToolsSettingsAccessor();

        setupMesonBuildConfiguration();
        setupNinjaBuildStep();

        ProjectManager::registerProjectType<MesonProject>(Constants::Project::MIMETYPE);
        FileIconProvider::registerIconOverlayForFilename(Constants::Icons::MESON, "meson.build");
        FileIconProvider::registerIconOverlayForFilename(Constants::Icons::MESON, "meson_options.txt");
    }

    class MesonProjectPluginPrivate *d = nullptr;
};

} // MesonProjectManager::Internal

#include "mesonprojectplugin.moc"
