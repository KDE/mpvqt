/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "mpvabstractitem.h"

#include <QThread>

#include "mpvcontroller.h"
#include "mpvrenderer.h"

MpvAbstractItem::MpvAbstractItem(QQuickItem *parent)
    : QQuickFramebufferObject(parent)
    , m_workerThread{new QThread}
    , m_mpvController{new MpvController}
{
    m_workerThread->start();
    m_mpvController->moveToThread(m_workerThread);
    m_mpv = m_mpvController->mpv();

    connect(m_workerThread, &QThread::finished, m_mpvController, &MpvController::deleteLater);
}

MpvAbstractItem::~MpvAbstractItem()
{
    if (m_mpv_gl) {
        mpv_render_context_free(m_mpv_gl);
    }
    mpv_set_wakeup_callback(m_mpv, nullptr, nullptr);

    m_workerThread->quit();
    m_workerThread->wait();
    m_workerThread->deleteLater();
    mpv_terminate_destroy(m_mpv);
}

QQuickFramebufferObject::Renderer *MpvAbstractItem::createRenderer() const
{
    return new MpvRenderer(const_cast<MpvAbstractItem *>(this));
}

int MpvAbstractItem::setProperty(const QString &property, const QVariant &value)
{
    return m_mpvController->setProperty(property, value);
}

int MpvAbstractItem::setPropertyAsync(const QString &property, const QVariant &value, int id)
{
    return m_mpvController->setPropertyAsync(property, value, id);
}

QVariant MpvAbstractItem::getProperty(const QString &property)
{
    return m_mpvController->getProperty(property);
}

int MpvAbstractItem::getPropertyAsync(const QString &property, int id)
{
    return m_mpvController->getPropertyAsync(property, id);
}

QVariant MpvAbstractItem::expandText(const QString &text)
{
    return m_mpvController->command(QStringList{QStringLiteral("expand-text"), text});
}

QVariant MpvAbstractItem::getCachedPropertyValue(const QString &property)
{
    if (!m_propertiesCache[property].isValid()) {
        auto value = getProperty(property);
        cachePropertyValue(property, value);
        return value;
    }
    return m_propertiesCache[property];
}

QVariant MpvAbstractItem::command(const QStringList &params)
{
    return m_mpvController->command(params);
}

int MpvAbstractItem::commandAsync(const QStringList &params, int id)
{
    return m_mpvController->commandAsync(params, id);
}

void MpvAbstractItem::observeProperty(const QString &property, mpv_format format, int id)
{
    mpv_observe_property(m_mpv, id, property.toUtf8().data(), format);
}

void MpvAbstractItem::cachePropertyValue(const QString &property, const QVariant &value)
{
    m_propertiesCache[property] = value;
}

#include "moc_mpvabstractitem.cpp"
