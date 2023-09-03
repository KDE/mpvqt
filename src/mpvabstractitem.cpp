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

void MpvAbstractItemPrivate::observeProperty(const QString &property, mpv_format format, int id)
{
    mpv_observe_property(m_mpv, id, property.toUtf8().data(), format);
}

void MpvAbstractItemPrivate::cachePropertyValue(const QString &property, const QVariant &value)
{
    m_propertiesCache[property] = value;
}

MpvAbstractItem::MpvAbstractItem(QQuickItem *parent)
    : QQuickFramebufferObject(parent)
    , d_ptr{std::make_unique<MpvAbstractItemPrivate>(this)}
{
    d_ptr->m_workerThread = new QThread;
    d_ptr->m_mpvController = new MpvController;
    d_ptr->m_workerThread->start();
    d_ptr->m_mpvController->moveToThread(d_ptr->m_workerThread);
    d_ptr->m_mpv = d_ptr->m_mpvController->mpv();

    connect(d_ptr->m_workerThread, &QThread::finished, d_ptr->m_mpvController, &MpvController::deleteLater);
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

int MpvAbstractItem::setProperty(const QString &property, const QVariant &value)
{
    return d_ptr->m_mpvController->setProperty(property, value);
}

int MpvAbstractItem::setPropertyAsync(const QString &property, const QVariant &value, int id)
{
    return d_ptr->m_mpvController->setPropertyAsync(property, value, id);
}

QVariant MpvAbstractItem::getProperty(const QString &property)
{
    return d_ptr->m_mpvController->getProperty(property);
}

int MpvAbstractItem::getPropertyAsync(const QString &property, int id)
{
    return d_ptr->m_mpvController->getPropertyAsync(property, id);
}

QVariant MpvAbstractItem::expandText(const QString &text)
{
    return d_ptr->m_mpvController->command(QStringList{QStringLiteral("expand-text"), text});
}

QVariant MpvAbstractItem::getCachedPropertyValue(const QString &property)
{
    if (!d_ptr->m_propertiesCache[property].isValid()) {
        auto value = getProperty(property);
        d_ptr->cachePropertyValue(property, value);
        return value;
    }
    return d_ptr->m_propertiesCache[property];
}

QVariant MpvAbstractItem::command(const QStringList &params)
{
    return d_ptr->m_mpvController->command(params);
}

int MpvAbstractItem::commandAsync(const QStringList &params, int id)
{
    return d_ptr->m_mpvController->commandAsync(params, id);
}

#include "moc_mpvabstractitem.cpp"
