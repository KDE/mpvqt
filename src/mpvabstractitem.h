/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef MPVABSTRACTITEM_H
#define MPVABSTRACTITEM_H

#include "mpvcontroller.h"
#include "mpvqt_export.h"

#include <QtQuick/QQuickFramebufferObject>

#include <mpv/client.h>
#include <mpv/render_gl.h>

class MpvController;
class MpvAbstractItemPrivate;

/**
 * MpvResourceManager is a lifecycle management utility designed
 * to handle the safe initialization and destruction of mpv_render_context.
 * Its primary goal is to ensure that GPU-bound resources are released
 * on the correct thread and that the underlying mpv_handle remains valid
 * until all rendering resources are fully cleaned up
 */
struct MpvResourceManager {
    // raw pointer to the mpv OpenGL render context
    mpv_render_context *mpvRenderContext{nullptr};
    // Shared pointer to the manager owning the mpv_handle
    // Ensures the core mpv instance outlives the rendering context
    std::shared_ptr<MpvHandleManager> mpvHandleManager;

    MpvResourceManager(mpv_render_context *c, std::shared_ptr<MpvHandleManager> owner)
        : mpvRenderContext(c)
        , mpvHandleManager(owner)
    {
    }

    /**
     * cleans up mpv's rendering context
     *
     * MUST be called from the Qt Render Thread (MpvRenderer)
     *
     * An OpenGL context must be current in the calling thread.
     * This must be the same context used to create the mpvRenderContext.
     */
    void freeContext()
    {
        if (mpvRenderContext) {
            mpv_render_context_set_update_callback(mpvRenderContext, nullptr, nullptr);
            mpv_render_context_free(mpvRenderContext);
            mpvRenderContext = nullptr;
        }
    }
};

class MPVQT_EXPORT MpvAbstractItem : public QQuickFramebufferObject
{
    Q_OBJECT
public:
    explicit MpvAbstractItem(QQuickItem *parent = nullptr);
    ~MpvAbstractItem();

    Renderer *createRenderer() const override;

    Q_INVOKABLE void observeProperty(const QString &property, mpv_format format, uint64_t id = 0);
    Q_INVOKABLE int unobserveProperty(uint64_t id);

    Q_INVOKABLE void setProperty(const QString &property, const QVariant &value);
    Q_INVOKABLE void setPropertyAsync(const QString &property, const QVariant &value, int id = 0);
    Q_INVOKABLE int setPropertyBlocking(const QString &property, const QVariant &value);

    Q_INVOKABLE QVariant getProperty(const QString &property);
    Q_INVOKABLE void getPropertyAsync(const QString &property, int id = 0);

    Q_INVOKABLE void command(const QStringList &params);
    Q_INVOKABLE QVariant commandBlocking(const QStringList &params);
    Q_INVOKABLE void commandAsync(const QStringList &params, int id = 0);

    Q_INVOKABLE QVariant expandText(const QString &text);
    Q_INVOKABLE void requestUpdateFromRenderer();

    friend class MpvRenderer;

Q_SIGNALS:
    void ready();

protected:
    MpvController *mpvController();

    std::unique_ptr<MpvAbstractItemPrivate> d_ptr;
};

#endif // MPVABSTRACTITEM_H
