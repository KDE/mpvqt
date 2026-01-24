/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef MPVRENDERER_H
#define MPVRENDERER_H

#include <QtQuick/QQuickFramebufferObject>

#include <mpv/render_gl.h>

class MpvAbstractItem;

class MpvRenderer : public QQuickFramebufferObject::Renderer
{
public:
    explicit MpvRenderer();
    ~MpvRenderer();

    // This function is called when a new FBO is needed.
    // This happens on the initial frame.
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;

    void render() override;
    void synchronize(QQuickFramebufferObject *item) override;
    void requestUpdate();

private:
    void createMpvRenderContext();
    QPointer<MpvAbstractItem> m_mpvAItem{nullptr};
    mpv_render_context *m_mpv_gl{nullptr};
    mpv_handle *m_mpv{nullptr};
    bool m_fboReady{false};
};

#endif // MPVRENDERER_H
