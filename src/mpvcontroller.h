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

#include <mpv/client.h>
#include <mpv/render_gl.h>

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
    MpvController(QObject *parent = nullptr);

    /**
     * Set the given property as mpv_node converted from the QVariant argument.
     *
     * @param `property` the name of the property
     * @return mpv error code (<0 on error, >= 0 on success)
     */
    int setProperty(const QString &property, const QVariant &value);
    int setPropertyAsync(const QString &property, const QVariant &value, int id);

    /**
     * Return the given property as mpv_node converted to QVariant,
     * or QVariant() on error.
     *
     * @param `property` the name of the property
     * @return the property value, or an ErrorReturn with the error code
     */
    QVariant getProperty(const QString &property);
    int getPropertyAsync(const QString &property, int id);

    /**
     * mpv_command_node() equivalent.
     *
     * @param `args` command arguments, with args[0] being the command name as string
     * @return the property value, or an ErrorReturn with the error code
     */
    QVariant command(const QVariant &params);

    /**
     * Return an error string from an ErrorReturn.
     */
    QString getError(int error);

    static void mpvEvents(void *ctx);
    void eventHandler();
    mpv_handle *mpv() const;

Q_SIGNALS:
    void propertyChanged(const QString &property, const QVariant &value);
    void getPropertyReply(const QVariant &value, int id);
    void setPropertyReply(int id);
    void fileStarted();
    void fileLoaded();
    void endFile(QString reason);
    void videoReconfig();

private:
    mpv_node_list *create_list(mpv_node *dst, bool is_map, int num);
    void setNode(mpv_node *dst, const QVariant &src);
    bool test_type(const QVariant &v, QMetaType::Type t);
    void free_node(mpv_node *dst);
    QVariant node_to_variant(const mpv_node *node);

    mpv_handle *m_mpv{nullptr};
};

#endif // MPVCONTROLLER_H
