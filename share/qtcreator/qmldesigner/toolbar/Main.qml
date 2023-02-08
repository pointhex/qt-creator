// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0+ OR GPL-3.0 WITH Qt-GPL-exception-1.0

import QtQuick
import QtQuick.Controls
import StudioControls 1.0 as StudioControls
import StudioTheme 1.0 as StudioTheme

import ToolBar 1.0

Rectangle {
    id: toolbarContainer
    color: StudioTheme.Values.themeToolbarBackground
    border.color: "#00000000"

    height: 56
    width: 2024

    ToolBarBackend {
        id: backend
    }

    Item {
        id: topToolbarOtherMode
        anchors.fill: parent
        visible: !backend.isInDesignMode

        ToolbarButton {
            id: homeOther
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10

            tooltip: backend.isDesignModeEnabled ? qsTr("Switch to Design Mode.") : qsTr("Switch to Welcome Mode.")
            buttonIcon: backend.isDesignModeEnabled ? StudioTheme.Constants.designMode_large : StudioTheme.Constants.home_large
            onClicked: backend.triggerModeChange()
        }

        Text {
            id: backTo
            visible: backend.isDesignModeEnabled
            anchors.verticalCenter: parent.verticalCenter
            text: qsTr("Return to Design")
            anchors.left: homeOther.right
            anchors.leftMargin: 10
            color: StudioTheme.Values.themeTextColor
        }
    }

    Item {
        id: topToolbar
        anchors.fill: parent
        visible: backend.isInDesignMode

        ToolbarButton {
            id: home
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10
            buttonIcon: StudioTheme.Constants.home_large
            onClicked: backend.triggerModeChange()
            tooltip: qsTr("Switch to Welcome Mode.")
        }

        ToolbarButton {
            id: runProject
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: home.right
            anchors.leftMargin: 10
            buttonIcon: StudioTheme.Constants.runProjOutline_large
            style: StudioTheme.ToolbarStyle {
                radius: StudioTheme.Values.smallRadius

                icon: StudioTheme.ControlStyle.IconColors {
                    idle: StudioTheme.Values.themeIdleGreen
                    hover: StudioTheme.Values.themeRunningGreen
                    interaction: "#ffffff"
                    disabled: "#636363"
                }

                background: StudioTheme.ControlStyle.BackgroundColors {
                    idle: StudioTheme.Values.themeControlBackground_toolbarIdle
                    hover: StudioTheme.Values.themeControlBackground_topToolbarHover
                    interaction: StudioTheme.Values.themeInteraction
                    disabled: StudioTheme.Values.themeControlBackground_toolbarIdle
                }

                border: StudioTheme.ControlStyle.BorderColors {
                    idle: StudioTheme.Values.themeControlBackground_toolbarIdle
                    hover: StudioTheme.Values.themeControlBackground_toolbarHover
                    interaction: StudioTheme.Values.themeInteraction
                    disabled: StudioTheme.Values.themeControlBackground_toolbarIdle
                }
            }

            onClicked: backend.runProject()
            tooltip: qsTr("Run Project")
        }

        ToolbarButton {
            id: livePreviewButton
            style: StudioTheme.Values.primaryToolbarStyle
            width: 96
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: runProject.right
            anchors.leftMargin: 10
            iconFont: StudioTheme.Constants.font
            buttonIcon: qsTr("Live Preview")

            onClicked: {
                livePreview.trigger()
            }

            MouseArea {
                acceptedButtons: Qt.RightButton
                anchors.fill: parent

                onClicked: {
                    var p = livePreviewButton.mapToGlobal(0, 0)
                    backend.showZoomMenu(p.x, p.y)
                }
            }

            ActionSubscriber {
                id: livePreview
                actionId: "LivePreview"
            }
        }

        StudioControls.TopLevelComboBox {
            id: currentFile
            style: StudioTheme.Values.toolbarStyle
            width: 320
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: livePreviewButton.right
            anchors.leftMargin: 10
            model: backend.documentModel
            currentIndex: backend.documentIndex

            onActivated: backend.openFileByIndex(index)
        }

        ToolbarButton {
            id: backButton
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: currentFile.right
            anchors.leftMargin: 10
            enabled: backend.canGoBack
            tooltip: qsTr("Go Back")
            buttonIcon: StudioTheme.Constants.previousFile_large
            iconRotation: 0

            onClicked: backend.goBackward()
        }

        ToolbarButton {
            id: forwardButton
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: backButton.right
            anchors.leftMargin: 10
            enabled: backend.canGoForward
            tooltip: qsTr("Go Forward")
            buttonIcon: StudioTheme.Constants.nextFile_large

            onClicked: backend.goForward()
        }

        ToolbarButton {
            id: closeButton
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: forwardButton.right
            anchors.leftMargin: 10
            tooltip: qsTr("Close")
            buttonIcon: StudioTheme.Constants.closeFile_large

            onClicked: backend.closeCurrentDocument()
        }

        CrumbleBar {
            id: flickable
            height: 36
            anchors.left: closeButton.right
            anchors.leftMargin: 10
            anchors.right: createComponent.left
            anchors.rightMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            model: CrumbleBarModel {
                id: crumbleBarModel
            }

            onClicked: crumbleBarModel.onCrumblePathElementClicked(index)
        }

        ToolbarButton {
            id: createComponent
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: enterComponent.left
            anchors.rightMargin: 10
            enabled: moveToComponentBackend.available
            tooltip: moveToComponentBackend.tooltip
            buttonIcon: StudioTheme.Constants.createComponent_large

            onClicked: moveToComponentBackend.trigger()

            ActionSubscriber {
                id: moveToComponentBackend
                actionId: "MakeComponent"
            }
        }

        ToolbarButton {
            id: enterComponent
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: workspaces.left
            anchors.rightMargin: 10
            enabled: goIntoComponentBackend.available
            tooltip: goIntoComponentBackend.tooltip
            buttonIcon: StudioTheme.Constants.editComponent_large

            onClicked: goIntoComponentBackend.trigger()

            ActionSubscriber {
                id: goIntoComponentBackend
                actionId: "GoIntoComponent"
            }
        }

        StudioControls.TopLevelComboBox {
            id: workspaces
            style: StudioTheme.Values.toolbarStyle
            width: 210
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: annotations.left
            anchors.rightMargin: 10
            model: backend.workspaces
            currentIndex: workspaces.find(backend.currentWorkspace)

            onActivated: backend.setCurrentWorkspace(workspaces.currentText)
        }

        ToolbarButton {
            id: annotations
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: shareButton.left
            anchors.rightMargin: 10
            tooltip: qsTr("Edit Annotations")
            buttonIcon: StudioTheme.Constants.annotations_large

            onClicked: backend.editGlobalAnnoation()
        }

        ToolbarButton {
            id: shareButton
            style: StudioTheme.Values.primaryToolbarStyle
            width: 96
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: moreItems.left
            anchors.rightMargin: 8
            iconFont: StudioTheme.Constants.font
            buttonIcon: qsTr("Share")

            onClicked: backend.shareApplicationOnline()
        }

        ToolbarButton {
            // this needs a pop-up panel where overflow toolbar content goes when toolbar is not wide enough
            id: moreItems
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 10
            tooltip: qsTr("More Items")
            buttonIcon: StudioTheme.Constants.more_medium
            enabled: false
            //onClicked: backend.editGlobalAnnoation()
        }
    }
}
