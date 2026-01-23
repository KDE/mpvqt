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
    Q_INVOKABLE int setPropertyBlocking(const QString &property, const QVariant &value);
    Q_INVOKABLE void setPropertyAsync(const QString &property, const QVariant &value, int id = 0);
    Q_INVOKABLE QVariant getProperty(const QString &property);
    Q_INVOKABLE void getPropertyAsync(const QString &property, int id = 0);
    Q_INVOKABLE QVariant commandBlocking(const QVariant &params);
    Q_INVOKABLE void commandAsync(const QStringList &params, int id = 0);
    Q_INVOKABLE QVariant expandText(const QString &text);
    Q_INVOKABLE int unobserveProperty(uint64_t id);
    Q_INVOKABLE void requestUpdateFromRenderer();

    friend class MpvRenderer;

Q_SIGNALS:
    void ready();
    void observeProperty(const QString &property, mpv_format format, uint64_t id = 0);
    void setProperty(const QString &property, const QVariant &value);
    void command(const QStringList &params);

protected:
    MpvController *mpvController();
    void resetMpvRenderContext();

    std::unique_ptr<MpvAbstractItemPrivate> d_ptr;
    bool m_fboReady{false};
    bool m_resetMpvRenderContext{false};
};

#endif // MPVABSTRACTITEM_H
