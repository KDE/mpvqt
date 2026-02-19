/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef MPVABSTRACTITEM_P_H_INCLUDED
#define MPVABSTRACTITEM_P_H_INCLUDED

#include "mpvabstractitem.h"

class MpvAbstractItemPrivate
{
public:
    explicit MpvAbstractItemPrivate(MpvAbstractItem *q);

    MpvAbstractItem *q_ptr;
    QThread *m_workerThread{nullptr};
    MpvController *m_mpvController{nullptr};
    bool m_isRendererReady{false};
    std::shared_ptr<MpvResourceManager> m_mpvResourceManager;
};

#endif // MPVABSTRACTITEM_P_H_INCLUDED
