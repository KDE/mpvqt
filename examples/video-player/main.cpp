/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>

#include <MpvAbstractItem>

int main(int argc, char *argv[])
{
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine(&app);
    const QUrl url(QStringLiteral("qrc:/Main.qml"));
    auto onObjectCreated = [url](const QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            QCoreApplication::exit(-1);
        }
    };
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, onObjectCreated, Qt::QueuedConnection);
    engine.loadFromModule("com.example.mpvqt", "Main");

    return app.exec();
}
