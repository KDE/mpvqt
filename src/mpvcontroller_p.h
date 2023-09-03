/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef MPVCONTROLLER_P_H_INCLUDED
#define MPVCONTROLLER_P_H_INCLUDED

class MpvControllerPrivate
{
public:
    MpvControllerPrivate(MpvController *q);
    MpvController *q_ptr;

    mpv_node_list *create_list(mpv_node *dst, bool is_map, int num);
    void setNode(mpv_node *dst, const QVariant &src);
    bool test_type(const QVariant &v, QMetaType::Type t);
    void free_node(mpv_node *dst);
    QVariant node_to_variant(const mpv_node *node);

    mpv_handle *m_mpv{nullptr};
};

#endif // MPVCONTROLLER_P_H_INCLUDED
