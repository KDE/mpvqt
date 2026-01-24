/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "mpvabstractitem.h"
#include "mpvabstractitem_p.h"

#include <QLoggingCategory>
#include <QQuickWindow>
#include <QThread>

#include "mpvcontroller.h"
#include "mpvrenderer.h"

Q_LOGGING_CATEGORY(MpvQt_MpvAbstractItem, "MpvQt.MpvAbstractItem")

MpvAbstractItemPrivate::MpvAbstractItemPrivate(MpvAbstractItem *q)
    : q_ptr(q)
{
}

MpvAbstractItem::MpvAbstractItem(QQuickItem *parent)
    : QQuickFramebufferObject(parent)
    , d_ptr{std::make_unique<MpvAbstractItemPrivate>(this)}
{
    if (QQuickWindow::graphicsApi() != QSGRendererInterface::OpenGL) {
        qCCritical(MpvQt_MpvAbstractItem) << "The graphics api must be set to opengl "
                                             "or mpv won't be able to render the video.\n"
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
                              &MpvController::observeProperty,
                              Qt::QueuedConnection,
                              property,
                              format,
                              id);
}

int MpvAbstractItem::unobserveProperty(uint64_t id)
{
    int result = 0;
    QMetaObject::invokeMethod(d_ptr->m_mpvController,
                              &MpvController::unobserveProperty,
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(int, result),
                              id);

    return result;
}

void MpvAbstractItem::setProperty(const QString &property, const QVariant &value)
{
    QMetaObject::invokeMethod(d_ptr->m_mpvController,
                              &MpvController::setProperty,
                              Qt::QueuedConnection,
                              property,
                              value);
}

int MpvAbstractItem::setPropertyBlocking(const QString &property, const QVariant &value)
{
    int error = 0;
    QMetaObject::invokeMethod(d_ptr->m_mpvController,
                              &MpvController::setProperty,
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(int, error),
                              property,
                              value);

    return error;
}

void MpvAbstractItem::setPropertyAsync(const QString &property, const QVariant &value, int id)
{
    QMetaObject::invokeMethod(d_ptr->m_mpvController,
                              &MpvController::setPropertyAsync,
                              Qt::QueuedConnection,
                              property,
                              value,
                              id);
}

QVariant MpvAbstractItem::getProperty(const QString &property)
{
    QVariant value;
    QMetaObject::invokeMethod(d_ptr->m_mpvController,
                              &MpvController::getProperty,
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(QVariant, value),
                              property);

    return value;
}

void MpvAbstractItem::getPropertyAsync(const QString &property, int id)
{
    QMetaObject::invokeMethod(d_ptr->m_mpvController,
                              &MpvController::getPropertyAsync,
                              Qt::QueuedConnection,
                              property,
                              id);
}

void MpvAbstractItem::command(const QStringList &params)
{
    QMetaObject::invokeMethod(d_ptr->m_mpvController,
                              &MpvController::command,
                              Qt::QueuedConnection,
                              params);
}

QVariant MpvAbstractItem::commandBlocking(const QStringList &params)
{
    QVariant value;
    QMetaObject::invokeMethod(d_ptr->m_mpvController,
                              &MpvController::command,
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(QVariant, value),
                              params);
    return value;
}

void MpvAbstractItem::commandAsync(const QStringList &params, int id)
{
    QMetaObject::invokeMethod(d_ptr->m_mpvController,
                              &MpvController::commandAsync,
                              Qt::QueuedConnection,
                              params,
                              id);
}

QVariant MpvAbstractItem::expandText(const QString &text)
{
    QVariant value;
    QMetaObject::invokeMethod(d_ptr->m_mpvController,
                              &MpvController::command,
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(QVariant, value),
                              QStringList() << QStringLiteral("expand-text") << text);
    return value;
}

// clang-format on

void MpvAbstractItem::requestUpdateFromRenderer()
{
    update();
}

#include "moc_mpvabstractitem.cpp"
