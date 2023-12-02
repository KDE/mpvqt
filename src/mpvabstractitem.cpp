/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "mpvabstractitem.h"
#include "mpvabstractitem_p.h"

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
    d_ptr->m_workerThread = new QThread;
    d_ptr->m_mpvController = new MpvController;
    d_ptr->m_workerThread->start();
    d_ptr->m_mpvController->moveToThread(d_ptr->m_workerThread);
    QMetaObject::invokeMethod(d_ptr->m_mpvController, "init", Qt::BlockingQueuedConnection);
    d_ptr->m_mpv = d_ptr->m_mpvController->mpv();

    connect(d_ptr->m_workerThread, &QThread::finished, d_ptr->m_mpvController, &MpvController::deleteLater);
    connect(this, &MpvAbstractItem::observeProperty, d_ptr->m_mpvController, &MpvController::observeProperty, Qt::QueuedConnection);
    connect(this, &MpvAbstractItem::setProperty, d_ptr->m_mpvController, &MpvController::setProperty, Qt::QueuedConnection);
    connect(this, &MpvAbstractItem::command, d_ptr->m_mpvController, &MpvController::command, Qt::QueuedConnection);
}

MpvAbstractItem::~MpvAbstractItem()
{
    if (d_ptr->m_mpv_gl) {
        mpv_render_context_free(d_ptr->m_mpv_gl);
    }
    mpv_set_wakeup_callback(d_ptr->m_mpv, nullptr, nullptr);

    d_ptr->m_workerThread->quit();
    d_ptr->m_workerThread->wait();
    d_ptr->m_workerThread->deleteLater();
    mpv_terminate_destroy(d_ptr->m_mpv);
}

QQuickFramebufferObject::Renderer *MpvAbstractItem::createRenderer() const
{
    return new MpvRenderer(const_cast<MpvAbstractItem *>(this));
}

MpvController *MpvAbstractItem::mpvController()
{
    return d_ptr->m_mpvController;
}

int MpvAbstractItem::setPropertyAsync(const QString &property, const QVariant &value, int id)
{
    return d_ptr->m_mpvController->setPropertyAsync(property, value, id);
}

QVariant MpvAbstractItem::getProperty(const QString &property)
{
    QVariant value;
    QMetaObject::invokeMethod(d_ptr->m_mpvController, "getProperty", Qt::BlockingQueuedConnection, Q_RETURN_ARG(QVariant, value), Q_ARG(QString, property));

    return value;
}

void MpvAbstractItem::getPropertyAsync(const QString &property, int id)
{
    QMetaObject::invokeMethod(d_ptr->m_mpvController, "getPropertyAsync", Qt::QueuedConnection, Q_ARG(QString, property), Q_ARG(int, id));
}

QVariant MpvAbstractItem::expandText(const QString &text)
{
    QVariant value;
    QMetaObject::invokeMethod(d_ptr->m_mpvController,
                              "command",
                              Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(QVariant, value),
                              Q_ARG(QVariant, QVariant::fromValue(QStringList{QStringLiteral("expand-text"), text})));
    return value;
}

QVariant MpvAbstractItem::commandBlocking(const QVariant &params)
{
    QVariant value;
    QMetaObject::invokeMethod(d_ptr->m_mpvController, "command", Qt::BlockingQueuedConnection, Q_RETURN_ARG(QVariant, value), Q_ARG(QVariant, params));
    return value;
}

int MpvAbstractItem::commandAsync(const QStringList &params, int id)
{
    return d_ptr->m_mpvController->commandAsync(params, id);
}

#include "moc_mpvabstractitem.cpp"
