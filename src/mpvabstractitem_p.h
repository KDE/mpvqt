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

    void observeProperty(const QString &property, mpv_format format, int id = 0);
    void cachePropertyValue(const QString &property, const QVariant &value);

    MpvAbstractItem *q_ptr;
    QThread *m_workerThread{nullptr};
    MpvController *m_mpvController{nullptr};
    mpv_handle *m_mpv{nullptr};
    mpv_render_context *m_mpv_gl{nullptr};
    QMap<QString, QVariant> m_propertiesCache;
};

#endif // MPVABSTRACTITEM_P_H_INCLUDED
