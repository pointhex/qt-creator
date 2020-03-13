/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#pragma once

#include "itestframework.h"

#include <QHash>

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

namespace Core { class Id; }

namespace Autotest {

class TestTreeItem;

namespace Internal {

class TestRunner;
struct TestSettings;
}

class IFrameworkSettings;
class ITestParser;
class TestTreeModel;

class TestFrameworkManager
{
public:
    static TestFrameworkManager *instance();
    virtual ~TestFrameworkManager();

    static ITestFramework *frameworkForId(Core::Id frameworkId);

    bool registerTestFramework(ITestFramework *framework);

    void activateFrameworksFromSettings(const Internal::TestSettings *settings);
    TestFrameworks registeredFrameworks() const;
    TestFrameworks sortedRegisteredFrameworks() const;
    TestFrameworks sortedActiveFrameworks() const;

    IFrameworkSettings *settingsForTestFramework(const Core::Id &frameworkId) const;
    void synchronizeSettings(QSettings *s);
    bool hasActiveFrameworks() const;

private:
    TestFrameworks activeFrameworks() const;
    explicit TestFrameworkManager();
    QHash<Core::Id, ITestFramework *> m_registeredFrameworks;
    QHash<Core::Id, IFrameworkSettings *> m_frameworkSettings;
    TestTreeModel *m_testTreeModel;
    Internal::TestRunner *m_testRunner;

    typedef QHash<Core::Id, ITestFramework *>::ConstIterator FrameworkIterator;
};

} // namespace Autotest
