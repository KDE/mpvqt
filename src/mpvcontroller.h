/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef MPVCONTROLLER_H
#define MPVCONTROLLER_H

#include "mpvqt_export.h"

#include <QMap>
#include <QObject>
#include <QVariant>

#include <mpv/client.h>
#include <mpv/render_gl.h>

#include <memory>

class MpvControllerPrivate;

/**
 * RAII wrapper that calls mpv_free_node_contents() on the pointer.
 */
struct node_autofree {
    mpv_node *ptr;
    explicit node_autofree(mpv_node *a_ptr)
        : ptr(a_ptr)
    {
    }
    ~node_autofree()
    {
        mpv_free_node_contents(ptr);
    }
};

/**
 * This is used to return error codes wrapped in QVariant for functions which
 * return QVariant.
 *
 * You can use get_error() or is_error() to extract the error status from a
 * QVariant value.
 */
struct ErrorReturn {
    /**
     * enum mpv_error value (or a value outside of it if ABI was extended)
     */
    int error;

    ErrorReturn()
        : error(0)
    {
    }
    explicit ErrorReturn(int err)
        : error(err)
    {
    }
};
Q_DECLARE_METATYPE(ErrorReturn)

class MPVQT_EXPORT MpvController : public QObject
{
    Q_OBJECT
public:
    explicit MpvController(QObject *parent = nullptr);

    /**
     * Return an error string from an ErrorReturn.
     */
    QString getError(int error);

    static void mpvEvents(void *ctx);
    void eventHandler();
    mpv_handle *mpv() const;

public Q_SLOTS:
    void init();

    /**
     * Get a notification whenever the given property changes. You will receive
     * updates as MPV_EVENT_PROPERTY_CHANGE.
     *
     * @param name The property name.
     * @param format see enum mpv_format. Can be MPV_FORMAT_NONE to omit values
     *               from the change events.
     */
    void observeProperty(const QString &property, mpv_format format, uint64_t id = 0);

    /**
     * Undo observeProperty(). This will remove all observed properties for
     * which the given number was passed as `id` to observeProperty.
     *
     * @param id the id that was passed to observeProperty
     * @return negative value is an error code, >=0 is number of removed properties
     *         on success (includes the case when 0 were removed)
     */
    int unobserveProperty(uint64_t id);

    /**
     * Set the given property as mpv_node converted from the QVariant argument.
     *
     * @param `property` the name of the property
     * @return mpv error code (<0 on error, >= 0 on success)
     */
    int setProperty(const QString &property, const QVariant &value);

    /**
     * Set a property asynchronously. The result of the operation as well
     * as the property data will be received in the MPV_EVENT_SET_PROPERTY_REPLY event,
     * handled by the eventHandler method. To do something after the property was set
     * connect to the asyncReply signal and use the id to identify the request.
     *
     * @param `property` the name of the property
     * @param `value` the value to set the property to
     * @param `id` used to associate requests with replies
     * @return error code if sending the request failed
     */
    int setPropertyAsync(const QString &property, const QVariant &value, int id = 0);

    /**
     * Return the given property as mpv_node converted to QVariant,
     * or QVariant() on error.
     *
     * @param `property` the name of the property
     * @return the property value, or an ErrorReturn with the error code
     */
    QVariant getProperty(const QString &property);

    /**
     * Get a property asynchronously. The result of the operation as well
     * as the property data will be received in the MPV_EVENT_GET_PROPERTY_REPLY event,
     * handled by the eventHandler method. To get the value connect to the asyncReply signal
     * and use the id to identify the request.
     *
     * @param `property` the name of the property
     * @param `id` used to associate requests with replies
     * @return error code if sending the request failed
     */
    int getPropertyAsync(const QString &property, int id = 0);

    /**
     * mpv_command_node() equivalent.
     *
     * @param `params` command arguments, with args[0] being the command name as string
     * @return the property value, or an ErrorReturn with the error code
     */
    QVariant command(const QVariant &params);

    /**
     * Run commands asynchronously. The result of the operation as well
     * as the property data will be received in the MPV_EVENT_COMMAND_REPLY event,
     * handled by the eventHandler method. To do something after the command was run,
     * connect to the asyncReply signal and use the id to identify the request.
     *
     * @param `params` command arguments, with args[0] being the command name as string
     * @param `id` used to associate requests with replies
     * @return error code (if parsing or queuing the command fails)
     */
    int commandAsync(const QVariant &params, int id = 0);

Q_SIGNALS:
    void propertyChanged(const QString &property, const QVariant &value);
    void asyncReply(const QVariant &data, mpv_event event);
    void fileStarted();
    void fileLoaded();
    void endFile(QString reason);
    void videoReconfig();

private:
    std::unique_ptr<MpvControllerPrivate> d_ptr;
};

Q_DECLARE_METATYPE(mpv_event)
#endif // MPVCONTROLLER_H
