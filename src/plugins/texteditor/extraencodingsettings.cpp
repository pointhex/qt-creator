// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "extraencodingsettings.h"

#include "behaviorsettingswidget.h"
#include "texteditortr.h"

#include <coreplugin/icore.h>

#include <utils/settingsutils.h>

// Keep this for compatibility reasons.
static const char kGroupPostfix[] = "EditorManager";
static const char kUtf8BomBehaviorKey[] = "Utf8BomBehavior";

using namespace Utils;

namespace TextEditor {

ExtraEncodingSettings::ExtraEncodingSettings() : m_utf8BomSetting(OnlyKeep)
{}

ExtraEncodingSettings::~ExtraEncodingSettings() = default;

void ExtraEncodingSettings::toSettings(const QString &category) const
{
    Q_UNUSED(category)

    Utils::toSettings(QLatin1String(kGroupPostfix), QString(), Core::ICore::settings(), this);
}

void ExtraEncodingSettings::fromSettings(const QString &category)
{
    Q_UNUSED(category)

    *this = ExtraEncodingSettings();
    Utils::fromSettings(QLatin1String(kGroupPostfix), QString(), Core::ICore::settings(), this);
}

QVariantMap ExtraEncodingSettings::toMap() const
{
    return {
        {kUtf8BomBehaviorKey, m_utf8BomSetting}
    };
}

void ExtraEncodingSettings::fromMap(const Storage &map)
{
    m_utf8BomSetting = (Utf8BomSetting)map.value(kUtf8BomBehaviorKey, m_utf8BomSetting).toInt();
}

bool ExtraEncodingSettings::equals(const ExtraEncodingSettings &s) const
{
    return m_utf8BomSetting == s.m_utf8BomSetting;
}

QStringList ExtraEncodingSettings::lineTerminationModeNames()
{
    return {Tr::tr("Unix (LF)"), Tr::tr("Windows (CRLF)")};
}

} // TextEditor
