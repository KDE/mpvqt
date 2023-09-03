/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "mpvrenderer.h"
#include "mpvabstractitem.h"
#include "mpvabstractitem_p.h"
#include "mpvcontroller.h"

#include <QGuiApplication>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QQuickWindow>

#if defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN) && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QtX11Extras/QX11Info>
#include <qpa/qplatformnativeinterface.h>
#endif

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
    QMetaObject::invokeMethod(static_cast<MpvAbstractItem *>(ctx), &MpvAbstractItem::update, Qt::QueuedConnection);
}

MpvRenderer::MpvRenderer(MpvAbstractItem *new_obj)
    : m_mpvAItem{new_obj}
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    m_mpvAItem->window()->setPersistentOpenGLContext(true);
#endif
    m_mpvAItem->window()->setPersistentSceneGraph(true);
}

void MpvRenderer::render()
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if (m_mpvAItem->window() != nullptr) {
        m_mpvAItem->window()->resetOpenGLState();
    }
#endif

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
    mpv_render_context_render(m_mpvAItem->d_ptr->m_mpv_gl, params);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if (m_mpvAItem->window() != nullptr) {
        m_mpvAItem->window()->resetOpenGLState();
    }
#endif
}

QOpenGLFramebufferObject *MpvRenderer::createFramebufferObject(const QSize &size)
{
    // init mpv_gl:
    if (!m_mpvAItem->d_ptr->m_mpv_gl) {
#if MPV_CLIENT_API_VERSION < MPV_MAKE_VERSION(2, 0)
        mpv_opengl_init_params gl_init_params{get_proc_address_mpv, nullptr, nullptr};
#else
        mpv_opengl_init_params gl_init_params{get_proc_address_mpv, nullptr};
#endif

        mpv_render_param display{MPV_RENDER_PARAM_INVALID, nullptr};
#if defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        if (QX11Info::isPlatformX11()) {
            display.type = MPV_RENDER_PARAM_X11_DISPLAY;
            display.data = QX11Info::display();
        }
        if (QGuiApplication::platformName() == QStringLiteral("wayland")) {
            display.type = MPV_RENDER_PARAM_WL_DISPLAY;
            display.data = (struct wl_display *)QGuiApplication::platformNativeInterface()->nativeResourceForWindow("display", nullptr);
        }
#else
        if (QGuiApplication::platformName() == QStringLiteral("xcb")) {
            display.type = MPV_RENDER_PARAM_X11_DISPLAY;
            display.data = qGuiApp->nativeInterface<QNativeInterface::QX11Application>()->display();
        }
        // TODO: figure out qt6 alternative for the wayland part
#endif
#endif
        mpv_render_param params[]{{MPV_RENDER_PARAM_API_TYPE, const_cast<char *>(MPV_RENDER_API_TYPE_OPENGL)},
                                  {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &gl_init_params},
                                  display,
                                  {MPV_RENDER_PARAM_INVALID, nullptr}};

        int result = mpv_render_context_create(&m_mpvAItem->d_ptr->m_mpv_gl, m_mpvAItem->d_ptr->m_mpv, params);
        if (result < 0) {
            qFatal("failed to initialize mpv GL context");
        }

        mpv_render_context_set_update_callback(m_mpvAItem->d_ptr->m_mpv_gl, on_mpv_redraw, m_mpvAItem);
        Q_EMIT m_mpvAItem->ready();
    }

    return QQuickFramebufferObject::Renderer::createFramebufferObject(size);
}
