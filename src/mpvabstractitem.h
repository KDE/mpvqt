/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef MPVABSTRACTITEM_H
#define MPVABSTRACTITEM_H

#include "mpvcontroller.h"

#include <QtQuick/QQuickFramebufferObject>
#include <mpv/client.h>
#include <mpv/render_gl.h>

class MpvController;

class MpvAbstractItem : public QQuickFramebufferObject
{
    Q_OBJECT
public:
    MpvAbstractItem(QQuickItem *parent = nullptr);
    ~MpvAbstractItem();

    Renderer *createRenderer() const override;
    Q_INVOKABLE int setPropertySynchronous(const QString &property, const QVariant &value);
    Q_INVOKABLE QVariant getPropertySynchronous(const QString &property);
    Q_INVOKABLE QVariant getCachedPropertyValue(const QString &property);
    Q_INVOKABLE QVariant synchronousCommand(const QStringList &params);
    Q_INVOKABLE QVariant expandText(const QString &text);

    friend class MpvRenderer;

Q_SIGNALS:
    void ready();
    void command(const QStringList &params);
    void setPropertyAsync(const QString &property, const QVariant &value, int id = 0);
    void getPropertyAsync(const QString &property, int id = 0);
    void setStringMpvProperty(const QString &property, const QVariant &value);

protected:
    void observeProperty(const QString &property, mpv_format format, int id = 0);
    void cachePropertyValue(const QString &property, const QVariant &value);

    QThread *m_workerThread{nullptr};
    MpvController *m_mpvController{nullptr};
    mpv_handle *m_mpv{nullptr};
    mpv_render_context *m_mpv_gl{nullptr};
    QMap<QString, QVariant> m_propertiesCache;
};

#endif // MPVABSTRACTITEM_H
