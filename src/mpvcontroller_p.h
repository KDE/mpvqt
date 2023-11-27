/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef MPVCONTROLLER_P_H_INCLUDED
#define MPVCONTROLLER_P_H_INCLUDED

#include "mpvcontroller.h"

class MpvControllerPrivate
{
public:
    explicit MpvControllerPrivate(MpvController *q);

    mpv_node_list *createList(mpv_node *dst, bool is_map, int num);
    void setNode(mpv_node *dst, const QVariant &src);
    bool testType(const QVariant &v, QMetaType::Type t);
    void freeNode(mpv_node *dst);
    QVariant nodeToVariant(const mpv_node *node);

    MpvController *q_ptr;
    mpv_handle *m_mpv{nullptr};
};

#endif // MPVCONTROLLER_P_H_INCLUDED
