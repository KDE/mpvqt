/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef MPVOBJECT_H
#define MPVOBJECT_H

#include <MpvAbstractItem>

class MpvRenderer;

class MpvItem : public MpvAbstractItem
{
    Q_OBJECT
public:
    explicit MpvItem(QQuickItem *parent = nullptr);
    ~MpvItem() = default;

    Q_PROPERTY(QString mediaTitle READ mediaTitle NOTIFY mediaTitleChanged)
    QString mediaTitle();

    Q_PROPERTY(double position READ position WRITE setPosition NOTIFY positionChanged)
    double position();
    void setPosition(double value);

    Q_PROPERTY(double duration READ duration NOTIFY durationChanged)
    double duration();

    Q_PROPERTY(QString formattedPosition READ formattedPosition NOTIFY positionChanged)
    QString formattedPosition() const;

    Q_PROPERTY(QString formattedDuration READ formattedDuration NOTIFY durationChanged)
    QString formattedDuration() const;

    Q_PROPERTY(bool pause READ pause WRITE setPause NOTIFY pauseChanged)
    bool pause();
    void setPause(bool value);

    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    int volume();
    void setVolume(int value);

    Q_PROPERTY(QUrl currentUrl READ currentUrl NOTIFY currentUrlChanged)
    QUrl currentUrl() const;

    Q_INVOKABLE void loadFile(const QString &file);

Q_SIGNALS:
    void mediaTitleChanged();
    void currentUrlChanged();
    void positionChanged();
    void durationChanged();
    void pauseChanged();
    void volumeChanged();

    void fileStarted();
    void fileLoaded();
    void endFile(QString reason);
    void videoReconfig();

private:
    void setupConnections();
    void onPropertyChanged(const QString &property, const QVariant &value);
    void onSetPropertyReply(int id);
    void onGetPropertyReply(const QVariant &value, int id);
    QString formatTime(const double time);

    double m_position{0.0};
    QString m_formattedPosition;
    double m_duration{0.0};
    QString m_formattedDuration;
    QUrl m_currentUrl;
};

#endif // MPVOBJECT_H
