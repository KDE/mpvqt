/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "mpvitem.h"

#include <MpvController>

#include "mpvproperties.h"

MpvItem::MpvItem(QQuickItem *parent)
    : MpvAbstractItem(parent)
{
    observeProperty(MpvProperties::self()->MediaTitle, MPV_FORMAT_STRING);
    observeProperty(MpvProperties::self()->Position, MPV_FORMAT_DOUBLE);
    observeProperty(MpvProperties::self()->Duration, MPV_FORMAT_DOUBLE);
    observeProperty(MpvProperties::self()->Pause, MPV_FORMAT_FLAG);
    observeProperty(MpvProperties::self()->Volume, MPV_FORMAT_INT64);

    setupConnections();

    // since this is async the effects are not immediately visible
    // to do something after the property was set do it in onSetPropertyReply
    // use the id to identify the correct call
    setPropertyAsync(QStringLiteral("volume"), 99, static_cast<int>(MpvItem::AsyncIds::SetVolume));
    setProperty(QStringLiteral("mute"), true);

    // since this is async the effects are not immediately visible
    // to get the value do it in onGetPropertyReply
    // use the id to identify the correct call
    getPropertyAsync(MpvProperties::self()->Volume, static_cast<int>(MpvItem::AsyncIds::GetVolume));
}

void MpvItem::setupConnections()
{
    // clang-format off
    connect(m_mpvController, &MpvController::propertyChanged,
            this, &MpvItem::onPropertyChanged, Qt::QueuedConnection);

    connect(m_mpvController, &MpvController::fileStarted,
            this, &MpvItem::fileStarted, Qt::QueuedConnection);

    connect(m_mpvController, &MpvController::fileLoaded,
            this, &MpvItem::fileLoaded, Qt::QueuedConnection);

    connect(m_mpvController, &MpvController::endFile,
            this, &MpvItem::endFile, Qt::QueuedConnection);

    connect(m_mpvController, &MpvController::videoReconfig,
            this, &MpvItem::videoReconfig, Qt::QueuedConnection);

    connect(m_mpvController, &MpvController::asyncReply,
            this, &MpvItem::onAsyncReply, Qt::QueuedConnection);
    // clang-format on
}

void MpvItem::onPropertyChanged(const QString &property, const QVariant &value)
{
    if (property == MpvProperties::self()->MediaTitle) {
        cachePropertyValue(property, value);
        Q_EMIT mediaTitleChanged();

    } else if (property == MpvProperties::self()->Position) {
        cachePropertyValue(property, value);
        m_formattedPosition = formatTime(value.toDouble());
        Q_EMIT positionChanged();

    } else if (property == MpvProperties::self()->Duration) {
        cachePropertyValue(property, value);
        m_formattedDuration = formatTime(value.toDouble());
        Q_EMIT durationChanged();

    } else if (property == MpvProperties::self()->Pause) {
        cachePropertyValue(property, value);
        Q_EMIT pauseChanged();

    } else if (property == MpvProperties::self()->Volume) {
        cachePropertyValue(property, value);
        Q_EMIT volumeChanged();

    }
}

void MpvItem::loadFile(const QString &file)
{
    auto url = QUrl::fromUserInput(file);
    if (m_currentUrl != url) {
        m_currentUrl = url;
        Q_EMIT currentUrlChanged();
    }

    Q_EMIT command(QStringList() << QStringLiteral("loadfile") << m_currentUrl.toLocalFile());
}

void MpvItem::onAsyncReply(const QVariant &data, int id)
{
    switch (static_cast<AsyncIds>(id)) {
    case AsyncIds::None: {
        break;
    }
    case AsyncIds::SetVolume: {
        qDebug() << "onSetPropertyReply" << id;
        break;
    }
    case AsyncIds::GetVolume: {
        qDebug() << "onGetPropertyReply" << id << data;
        break;
    }
    case AsyncIds::ExpandText: {
        qDebug() << "onGetPropertyReply" << id << data;
        break;
    }
    }
}

QString MpvItem::formatTime(const double time)
{
    int totalNumberOfSeconds = static_cast<int>(time);
    int seconds = totalNumberOfSeconds % 60;
    int minutes = (totalNumberOfSeconds / 60) % 60;
    int hours = (totalNumberOfSeconds / 60 / 60);

    QString timeString =
        QStringLiteral("%1:%2:%3").arg(hours, 2, 10, QLatin1Char('0')).arg(minutes, 2, 10, QLatin1Char('0')).arg(seconds, 2, 10, QLatin1Char('0'));

    return timeString;
}

QString MpvItem::mediaTitle()
{
    return getCachedPropertyValue(MpvProperties::self()->MediaTitle).toString();
}

double MpvItem::position()
{
    return getCachedPropertyValue(MpvProperties::self()->Position).toDouble();
}

void MpvItem::setPosition(double value)
{
    if (qFuzzyCompare(value, position())) {
        return;
    }
    Q_EMIT setPropertyAsync(MpvProperties::self()->Position, value);
}

double MpvItem::duration()
{
    return getCachedPropertyValue(MpvProperties::self()->Duration).toDouble();
}

bool MpvItem::pause()
{
    return getCachedPropertyValue(MpvProperties::self()->Pause).toBool();
}

void MpvItem::setPause(bool value)
{
    if (value == pause()) {
        return;
    }
    Q_EMIT setPropertyAsync(MpvProperties::self()->Pause, value);
}

int MpvItem::volume()
{
    return getCachedPropertyValue(MpvProperties::self()->Volume).toInt();
}

void MpvItem::setVolume(int value)
{
    if (value == volume()) {
        return;
    }
    Q_EMIT setPropertyAsync(MpvProperties::self()->Volume, value);
}

QString MpvItem::formattedDuration() const
{
    return m_formattedDuration;
}

QString MpvItem::formattedPosition() const
{
    return m_formattedPosition;
}

QUrl MpvItem::currentUrl() const
{
    return m_currentUrl;
}

#include "moc_mpvitem.cpp"
