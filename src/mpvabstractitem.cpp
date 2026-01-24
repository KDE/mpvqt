/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "mpvabstractitem.h"
#include "mpvabstractitem_p.h"

#include <QQuickWindow>
#include <QThread>

#include "mpvcontroller.h"
#include "mpvrenderer.h"

MpvAbstractItemPrivate::MpvAbstractItemPrivate(MpvAbstractItem *q)
    : q_ptr(q)
{
}

MpvAbstractItem::MpvAbstractItem(QQuickItem *parent)
    : QQuickFramebufferObject(parent)
    , d_ptr{std::make_unique<MpvAbstractItemPrivate>(this)}
{
    if (QQuickWindow::graphicsApi() != QSGRendererInterface::OpenGL) {
        qDebug() << "MpvAbstractItem: "
                    "The graphics api must be set to opengl or mpv won't be able to render the video.\n"
                    "QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL)\n"
                    "The call to the function must happen before constructing "
                    "the first QQuickWindow in the application.";
    }

    d_ptr->m_workerThread = new QThread(this);
    d_ptr->m_mpvController = new MpvController;

    connect(d_ptr->m_workerThread, &QThread::finished, d_ptr->m_mpvController, &MpvController::deleteLater);
    connect(d_ptr->m_mpvController, &MpvController::initialized, this, [this]() {
        d_ptr->m_mpv = d_ptr->m_mpvController->mpv();
        update();
    });

    d_ptr->m_mpvController->moveToThread(d_ptr->m_workerThread);
    d_ptr->m_workerThread->start();

    QMetaObject::invokeMethod(d_ptr->m_mpvController, &MpvController::init, Qt::QueuedConnection);
}

MpvAbstractItem::~MpvAbstractItem()
{
    mpv_set_wakeup_callback(d_ptr->m_mpv, nullptr, nullptr);
    d_ptr->m_workerThread->quit();
    d_ptr->m_workerThread->wait();
    mpv_terminate_destroy(d_ptr->m_mpv);
    d_ptr->m_workerThread->deleteLater();
}

QQuickFramebufferObject::Renderer *MpvAbstractItem::createRenderer() const
{
    return new MpvRenderer();
}

MpvController *MpvAbstractItem::mpvController()
{
    return d_ptr->m_mpvController;
}

// clang-format off

void MpvAbstractItem::observeProperty(const QString &property, mpv_format format, uint64_t id)
{
    QMetaObject::invokeMethod(d_ptr->m_mpvController,
                              "observeProperty",
                              Qt::QueuedConnection,
                              Q_ARG(QString, property),
                              Q_ARG(mpv_format, format),
                              Q_ARG(uint64_t, id));
}

int MpvAbstractItem::unobserveProperty(uint64_t id)
{
    int result = 0;
    QMetaObject::invokeMethod(d_ptr->m_mpvController,
                              "unobserveProperty",
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(int, result),
                              Q_ARG(uint64_t, id));

    return result;
}

void MpvAbstractItem::setProperty(const QString &property, const QVariant &value)
{
    QMetaObject::invokeMethod(d_ptr->m_mpvController,
                              "setProperty",
                              Qt::QueuedConnection,
                              Q_ARG(QString, property),
                              Q_ARG(QVariant, value));
}

int MpvAbstractItem::setPropertyBlocking(const QString &property, const QVariant &value)
{
    int error = 0;
    QMetaObject::invokeMethod(d_ptr->m_mpvController,
                              "setProperty",
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(int, error),
                              Q_ARG(QString, property),
                              Q_ARG(QVariant, value));

    return error;
}

void MpvAbstractItem::setPropertyAsync(const QString &property, const QVariant &value, int id)
{
    QMetaObject::invokeMethod(d_ptr->m_mpvController,
                              "setPropertyAsync",
                              Qt::QueuedConnection,
                              Q_ARG(QString, property),
                              Q_ARG(QVariant, value),
                              Q_ARG(int, id));
}

QVariant MpvAbstractItem::getProperty(const QString &property)
{
    QVariant value;
    QMetaObject::invokeMethod(d_ptr->m_mpvController,
                              "getProperty",
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(QVariant, value),
                              Q_ARG(QString, property));

    return value;
}

void MpvAbstractItem::getPropertyAsync(const QString &property, int id)
{
    QMetaObject::invokeMethod(d_ptr->m_mpvController,
                              "getPropertyAsync",
                              Qt::QueuedConnection,
                              Q_ARG(QString, property),
                              Q_ARG(int, id));
}

void MpvAbstractItem::command(const QStringList &params)
{
    QMetaObject::invokeMethod(d_ptr->m_mpvController,
                              "command",
                              Qt::QueuedConnection,
                              Q_ARG(QStringList, params));
}

QVariant MpvAbstractItem::commandBlocking(const QVariant &params)
{
    QVariant value;
    QMetaObject::invokeMethod(d_ptr->m_mpvController,
                              "command",
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(QVariant, value),
                              Q_ARG(QVariant, params));
    return value;
}

void MpvAbstractItem::commandAsync(const QStringList &params, int id)
{
    QMetaObject::invokeMethod(d_ptr->m_mpvController,
                              "commandAsync",
                              Qt::QueuedConnection,
                              Q_ARG(QVariant, params),
                              Q_ARG(int, id));
}

QVariant MpvAbstractItem::expandText(const QString &text)
{
    QVariant value;
    QMetaObject::invokeMethod(d_ptr->m_mpvController,
                              "command",
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(QVariant, value),
                              Q_ARG(QStringList, QStringList() << QStringLiteral("expand-text") << text));
    return value;
}

// clang-format on

void MpvAbstractItem::requestUpdateFromRenderer()
{
    update();
}

#include "moc_mpvabstractitem.cpp"
