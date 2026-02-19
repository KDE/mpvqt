/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "mpvrenderer.h"

#include <QGuiApplication>
#include <QLoggingCategory>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QQuickWindow>

#include "mpvabstractitem.h"
#include "mpvabstractitem_p.h"
#include "mpvcontroller.h"

Q_STATIC_LOGGING_CATEGORY(MpvQt_MpvRenderer, "MpvQt.MpvRenderer")

static void *get_proc_address_mpv(void *ctx, const char *name)
{
    Q_UNUSED(ctx)

    QOpenGLContext *glctx = QOpenGLContext::currentContext();
    if (!glctx) {
        return nullptr;
    }

    return reinterpret_cast<void *>(glctx->getProcAddress(QByteArray(name)));
}

void on_mpv_redraw(void *ctx)
{
    auto *r = static_cast<MpvRenderer *>(ctx);
    r->requestUpdate();
}

MpvRenderer::MpvRenderer()
{
}

MpvRenderer::~MpvRenderer()
{
    if (m_mpvResourceManager) {
        m_mpvResourceManager->freeContext();
    }
}

void MpvRenderer::synchronize(QQuickFramebufferObject *item)
{
    MpvAbstractItem *mpvAItem = static_cast<MpvAbstractItem *>(item);
    m_mpvAItem = mpvAItem;

    if (!m_mpvResourceManager) {
        m_mpvResourceManager = mpvAItem->d_ptr->m_mpvResourceManager;
    }

    if (mpvAItem->d_ptr->m_isRendererReady != m_isFramebufferReady) {
        mpvAItem->d_ptr->m_isRendererReady = m_isFramebufferReady;

        Q_EMIT mpvAItem->ready();
    }
}

void MpvRenderer::render()
{
    QOpenGLFramebufferObject *fbo = framebufferObject();
    mpv_opengl_fbo mpfbo;
    mpfbo.fbo = static_cast<int>(fbo->handle());
    mpfbo.w = fbo->width();
    mpfbo.h = fbo->height();
    mpfbo.internal_format = 0;

    int flip_y{0};

    mpv_render_param params[] = {// Specify the default framebuffer (0) as target. This will
                                 // render onto the entire screen. If you want to show the video
                                 // in a smaller rectangle or apply fancy transformations, you'll
                                 // need to render into a separate FBO and draw it manually.
                                 {MPV_RENDER_PARAM_OPENGL_FBO, &mpfbo},
                                 {MPV_RENDER_PARAM_FLIP_Y, &flip_y},
                                 {MPV_RENDER_PARAM_INVALID, nullptr}};
    // See render_gl.h on what OpenGL environment mpv expects, and
    // other API details.
    int result = mpv_render_context_render(m_mpvResourceManager->mpvRenderContext, params);
    if (result < 0) {
        qCWarning(MpvQt_MpvRenderer) << "mpv_render_context_render failed:" << MpvController::getError(result);
        return;
    }
}

QOpenGLFramebufferObject *MpvRenderer::createFramebufferObject(const QSize &size)
{
    if (m_mpvResourceManager && !m_mpvResourceManager->mpvRenderContext) {
        m_mpvResourceManager->mpvRenderContext = createMpvRenderContext();
        m_isFramebufferReady = true;
    }

    return QQuickFramebufferObject::Renderer::createFramebufferObject(size);
}

mpv_render_context *MpvRenderer::createMpvRenderContext()
{
    mpv_opengl_init_params gl_init_params{get_proc_address_mpv, nullptr};

    mpv_render_param display{MPV_RENDER_PARAM_INVALID, nullptr};
#if defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN) && !defined(Q_OS_ANDROID) && !defined(Q_OS_HAIKU)
    if (QGuiApplication::platformName() == QStringLiteral("xcb")) {
        display.type = MPV_RENDER_PARAM_X11_DISPLAY;
        display.data = qGuiApp->nativeInterface<QNativeInterface::QX11Application>()->display();
    }

    if (QGuiApplication::platformName() == QStringLiteral("wayland")) {
        display.type = MPV_RENDER_PARAM_WL_DISPLAY;
        display.data = qGuiApp->nativeInterface<QNativeInterface::QWaylandApplication>()->display();
    }
#endif

    mpv_render_param params[]{{MPV_RENDER_PARAM_API_TYPE, const_cast<char *>(MPV_RENDER_API_TYPE_OPENGL)},
                              {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &gl_init_params},
                              display,
                              {MPV_RENDER_PARAM_INVALID, nullptr}};

    mpv_render_context *renderCtx = nullptr;
    mpv_handle *handle = m_mpvResourceManager->mpvHandleManager->mpvHandle;
    int result = mpv_render_context_create(&renderCtx, handle, params);
    if (result < 0) {
        qCritical() << "failed to initialize mpv GL context:" << mpv_error_string(result);
        return nullptr;
    }

    mpv_render_context_set_update_callback(renderCtx, on_mpv_redraw, this);
    return renderCtx;
}

void MpvRenderer::requestUpdate()
{
    if (m_mpvAItem) {
        QMetaObject::invokeMethod(m_mpvAItem.data(), "requestUpdateFromRenderer", Qt::QueuedConnection);
    }
}
