/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtXmlPatterns module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia. For licensing terms and
** conditions see http://qt.digia.com/licensing. For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_StaticNamespacesContainer_H
#define Patternist_StaticNamespacesContainer_H

#include <private/qfunctioncall_p.h>

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short A helper subclass that stores a NamespaceResolver for the static
     * namespaces.
     *
     * This is used by functionality which needs to resolve names against the
     * statically known namespaces, at runtime. A good example of this is @c
     * function-available().
     *
     * The resolver is accessed through staticNamespaces(), which will be
     * available after the typeCheck() stage.
     *
     * This class must be subclassed.
     *
     * @author Frans Englich <frans.englich@nokia.com>
     * @ingroup Patternist_functions
     */
    class StaticNamespacesContainer : public FunctionCall
    {
    public:
        /**
         * Reimplemented to store data from the @p context.
         */
        virtual Expression::Ptr typeCheck(const StaticContext::Ptr &context,
                                          const SequenceType::Ptr &reqType);

    protected:
        /**
         * Before typeCheck(), behavior of this function is undefined. After
         * typeCheck(), this function guarantees to return a valid pointer.
         */
        inline const NamespaceResolver::Ptr &staticNamespaces() const
        {
            Q_ASSERT(m_resolver);
            return m_resolver;
        }

        /**
         * This constructor only exists to ensure this class is subclassed.
         */
        inline StaticNamespacesContainer()
        {
        }

    private:
        NamespaceResolver::Ptr m_resolver;
    };
}

QT_END_NAMESPACE

#endif
