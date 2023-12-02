/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef MPVABSTRACTITEM_H
#define MPVABSTRACTITEM_H

#include "mpvqt_export.h"

#include <QtQuick/QQuickFramebufferObject>
#include <mpv/client.h>
#include <mpv/render_gl.h>

class MpvController;
class MpvAbstractItemPrivate;

class MPVQT_EXPORT MpvAbstractItem : public QQuickFramebufferObject
{
    Q_OBJECT
public:
    explicit MpvAbstractItem(QQuickItem *parent = nullptr);
    ~MpvAbstractItem();

    Renderer *createRenderer() const override;
    Q_INVOKABLE int setPropertyAsync(const QString &property, const QVariant &value, int id = 0);
    Q_INVOKABLE QVariant getProperty(const QString &property);
    Q_INVOKABLE int getPropertyAsync(const QString &property, int id = 0);
    Q_INVOKABLE int commandAsync(const QStringList &params, int id = 0);
    Q_INVOKABLE QVariant expandText(const QString &text);

    friend class MpvRenderer;

Q_SIGNALS:
    void ready();
    void observeProperty(const QString &property, mpv_format format);
    void setProperty(const QString &property, const QVariant &value);
    void command(const QStringList &params);

protected:
    MpvController *mpvController();

    std::unique_ptr<MpvAbstractItemPrivate> d_ptr;
};

#endif // MPVABSTRACTITEM_H
