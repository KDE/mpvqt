/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "mpvcontroller.h"
#include "mpvcontroller_p.h"

#include <QVariant>
#include <QDebug>

#include <clocale>

MpvControllerPrivate::MpvControllerPrivate(MpvController *q)
    : q_ptr(q)
{
}

mpv_node_list *MpvControllerPrivate::createList(mpv_node *dst, bool is_map, int num)
{
    dst->format = is_map ? MPV_FORMAT_NODE_MAP : MPV_FORMAT_NODE_ARRAY;
    mpv_node_list *list = new mpv_node_list();
    dst->u.list = list;
    if (!list) {
        freeNode(dst);
        return nullptr;
    }
    list->values = new mpv_node[num]();
    if (!list->values) {
        freeNode(dst);
        return nullptr;
    }
    if (is_map) {
        list->keys = new char *[num]();
        if (!list->keys) {
            freeNode(dst);
            return nullptr;
        }
    }
    return list;
}

void MpvControllerPrivate::setNode(mpv_node *dst, const QVariant &src)
{
    if (testType(src, QMetaType::QString)) {
        dst->format = MPV_FORMAT_STRING;
        dst->u.string = qstrdup(src.toString().toUtf8().data());
        if (!dst->u.string) {
            dst->format = MPV_FORMAT_NONE;
        }
    } else if (testType(src, QMetaType::Bool)) {
        dst->format = MPV_FORMAT_FLAG;
        dst->u.flag = src.toBool() ? 1 : 0;
    } else if (testType(src, QMetaType::Int) || testType(src, QMetaType::LongLong) || testType(src, QMetaType::UInt) || testType(src, QMetaType::ULongLong)) {
        dst->format = MPV_FORMAT_INT64;
        dst->u.int64 = src.toLongLong();
    } else if (testType(src, QMetaType::Double)) {
        dst->format = MPV_FORMAT_DOUBLE;
        dst->u.double_ = src.toDouble();
    } else if (src.canConvert<QVariantList>()) {
        QVariantList qlist = src.toList();
        mpv_node_list *list = createList(dst, false, qlist.size());
        if (!list) {
            dst->format = MPV_FORMAT_NONE;
            return;
        }
        list->num = qlist.size();
        for (int n = 0; n < qlist.size(); ++n) {
            setNode(&list->values[n], qlist[n]);
        }
    } else if (src.canConvert<QVariantMap>()) {
        QVariantMap qmap = src.toMap();
        mpv_node_list *list = createList(dst, true, qmap.size());
        if (!list) {
            dst->format = MPV_FORMAT_NONE;
            return;
        }
        list->num = qmap.size();
        int n = 0;
        for (auto it = qmap.constKeyValueBegin(); it != qmap.constKeyValueEnd(); ++it) {
            list->keys[n] = qstrdup(it.operator*().first.toUtf8().data());
            if (!list->keys[n]) {
                freeNode(dst);
                dst->format = MPV_FORMAT_NONE;
                return;
            }
            setNode(&list->values[n], it.operator*().second);
            ++n;
        }
    } else {
        dst->format = MPV_FORMAT_NONE;
    }
    return;
}

bool MpvControllerPrivate::testType(const QVariant &v, QMetaType::Type t)
{
    // The Qt docs say: "Although this function is declared as returning
    // QVariant::Type(obsolete), the return value should be interpreted
    // as QMetaType::Type." So a cast is needed to avoid warnings.
    return v.typeId() == t;
}

void MpvControllerPrivate::freeNode(mpv_node *dst)
{
    switch (dst->format) {
    case MPV_FORMAT_STRING:
        delete[] dst->u.string;
        break;
    case MPV_FORMAT_NODE_ARRAY:
    case MPV_FORMAT_NODE_MAP: {
        mpv_node_list *list = dst->u.list;
        if (list) {
            for (int n = 0; n < list->num; ++n) {
                if (list->keys) {
                    delete[] list->keys[n];
                }
                if (list->values) {
                    freeNode(&list->values[n]);
                }
            }
            delete[] list->keys;
            delete[] list->values;
        }
        delete list;
        break;
    }
    default:;
    }
    dst->format = MPV_FORMAT_NONE;
}

inline QVariant MpvControllerPrivate::nodeToVariant(const mpv_node *node)
{
    switch (node->format) {
    case MPV_FORMAT_STRING:
        return QVariant(QString::fromUtf8(node->u.string));
    case MPV_FORMAT_FLAG:
        return QVariant(static_cast<bool>(node->u.flag));
    case MPV_FORMAT_INT64:
        return QVariant(static_cast<qlonglong>(node->u.int64));
    case MPV_FORMAT_DOUBLE:
        return QVariant(node->u.double_);
    case MPV_FORMAT_NODE_ARRAY: {
        mpv_node_list *list = node->u.list;
        QVariantList qlist;
        for (int n = 0; n < list->num; ++n) {
            qlist.append(nodeToVariant(&list->values[n]));
        }
        return QVariant(qlist);
    }
    case MPV_FORMAT_NODE_MAP: {
        mpv_node_list *list = node->u.list;
        QVariantMap qmap;
        for (int n = 0; n < list->num; ++n) {
            qmap.insert(QString::fromUtf8(list->keys[n]), nodeToVariant(&list->values[n]));
        }
        return QVariant(qmap);
    }
    default: // MPV_FORMAT_NONE, unknown values (e.g. future extensions)
        return QVariant();
    }
}

MpvController::MpvController(QObject *parent)
    : QObject(parent)
{
}

void MpvController::init()
{
    d_ptr = std::make_unique<MpvControllerPrivate>(this);
    // Qt sets the locale in the QGuiApplication constructor, but libmpv
    // requires the LC_NUMERIC category to be set to "C", so change it back.
    std::setlocale(LC_NUMERIC, "C");

    d_ptr->m_mpv = mpv_create();
    if (!d_ptr->m_mpv) {
        qFatal("could not create mpv context");
    }
    if (mpv_initialize(d_ptr->m_mpv) < 0) {
        qFatal("could not initialize mpv context");
    }
    mpv_set_wakeup_callback(d_ptr->m_mpv, MpvController::mpvEvents, this);
}

void MpvController::mpvEvents(void *ctx)
{
    QMetaObject::invokeMethod(static_cast<MpvController *>(ctx), &MpvController::eventHandler, Qt::QueuedConnection);
}

void MpvController::eventHandler()
{
    while (d_ptr->m_mpv) {
        mpv_event *event = mpv_wait_event(d_ptr->m_mpv, 0);
        if (event->event_id == MPV_EVENT_NONE) {
            break;
        }
        switch (event->event_id) {
        case MPV_EVENT_START_FILE: {
            Q_EMIT fileStarted();
            break;
        }

        case MPV_EVENT_FILE_LOADED: {
            Q_EMIT fileLoaded();
            break;
        }

        case MPV_EVENT_END_FILE: {
            auto prop = static_cast<mpv_event_end_file *>(event->data);
            if (prop->reason == MPV_END_FILE_REASON_EOF) {
                Q_EMIT endFile(QStringLiteral("eof"));
            } else if (prop->reason == MPV_END_FILE_REASON_ERROR) {
                Q_EMIT endFile(QStringLiteral("error"));
            }
            break;
        }

        case MPV_EVENT_VIDEO_RECONFIG: {
            Q_EMIT videoReconfig();
            break;
        }

        case MPV_EVENT_GET_PROPERTY_REPLY: {
            mpv_event_property *prop = static_cast<mpv_event_property *>(event->data);
            auto data = d_ptr->nodeToVariant(reinterpret_cast<mpv_node *>(prop->data));
            Q_EMIT asyncReply(data, {*event});
            break;
        }

        case MPV_EVENT_SET_PROPERTY_REPLY: {
            Q_EMIT asyncReply(QVariant(), {*event});
            break;
        }

        case MPV_EVENT_COMMAND_REPLY: {
            mpv_event_property *prop = static_cast<mpv_event_property *>(event->data);
            auto data = d_ptr->nodeToVariant(reinterpret_cast<mpv_node *>(prop));
            Q_EMIT asyncReply(data, {*event});
            break;
        }

        case MPV_EVENT_PROPERTY_CHANGE: {
            mpv_event_property *prop = static_cast<mpv_event_property *>(event->data);
            QVariant data;
            switch (prop->format) {
            case MPV_FORMAT_DOUBLE:
                data = *reinterpret_cast<double *>(prop->data);
                break;
            case MPV_FORMAT_STRING:
                data = QString::fromStdString(*reinterpret_cast<char **>(prop->data));
                break;
            case MPV_FORMAT_INT64:
                data = qlonglong(*reinterpret_cast<int64_t *>(prop->data));
                break;
            case MPV_FORMAT_FLAG:
                data = *reinterpret_cast<bool *>(prop->data);
                break;
            case MPV_FORMAT_NODE:
                data = d_ptr->nodeToVariant(reinterpret_cast<mpv_node *>(prop->data));
                break;
            case MPV_FORMAT_NONE:
            case MPV_FORMAT_OSD_STRING:
            case MPV_FORMAT_NODE_ARRAY:
            case MPV_FORMAT_NODE_MAP:
            case MPV_FORMAT_BYTE_ARRAY:
                break;
            }
            Q_EMIT propertyChanged(QString::fromStdString(prop->name), data);
            break;
        }
        case MPV_EVENT_NONE:
        case MPV_EVENT_SHUTDOWN:
        case MPV_EVENT_LOG_MESSAGE:
        case MPV_EVENT_CLIENT_MESSAGE:
        case MPV_EVENT_AUDIO_RECONFIG:
        case MPV_EVENT_SEEK:
        case MPV_EVENT_PLAYBACK_RESTART:
        case MPV_EVENT_QUEUE_OVERFLOW:
        case MPV_EVENT_HOOK:
#if MPV_ENABLE_DEPRECATED
        case MPV_EVENT_IDLE:
        case MPV_EVENT_TICK:
#endif
            break;
        }
    }
}

mpv_handle *MpvController::mpv() const
{
    return d_ptr->m_mpv;
}

void MpvController::observeProperty(const QString &property, mpv_format format)
{
    mpv_observe_property(mpv(), 0, property.toUtf8().data(), format);
}

int MpvController::setProperty(const QString &property, const QVariant &value)
{
    mpv_node node;
    d_ptr->setNode(&node, value);
    return mpv_set_property(d_ptr->m_mpv, property.toUtf8().constData(), MPV_FORMAT_NODE, &node);
}

int MpvController::setPropertyAsync(const QString &property, const QVariant &value, int id)
{
    mpv_node node;
    d_ptr->setNode(&node, value);
    int err = mpv_set_property_async(d_ptr->m_mpv, id, property.toUtf8().constData(), MPV_FORMAT_NODE, &node);
    return err;
}

QVariant MpvController::getProperty(const QString &property)
{
    mpv_node node;
    int err = mpv_get_property(d_ptr->m_mpv, property.toUtf8().constData(), MPV_FORMAT_NODE, &node);
    if (err < 0) {
        return QVariant::fromValue(ErrorReturn(err));
    }
    node_autofree f(&node);
    return d_ptr->nodeToVariant(&node);
}

int MpvController::getPropertyAsync(const QString &property, int id)
{
    int err = mpv_get_property_async(d_ptr->m_mpv, id, property.toUtf8().constData(), MPV_FORMAT_NODE);
    return err;
}

QVariant MpvController::command(const QVariant &params)
{
    mpv_node node;
    d_ptr->setNode(&node, params);
    mpv_node result;
    int err = mpv_command_node(d_ptr->m_mpv, &node, &result);
    if (err < 0) {
        qDebug() << getError(err) << params;
        return QVariant::fromValue(ErrorReturn(err));
    }
    node_autofree f(&result);
    return d_ptr->nodeToVariant(&result);
}

int MpvController::commandAsync(const QVariant &params, int id)
{
    mpv_node node;
    d_ptr->setNode(&node, params);
    return mpv_command_node_async(d_ptr->m_mpv, id, &node);
}

QString MpvController::getError(int error)
{
    ErrorReturn err{error};
    switch (err.error) {
    case MPV_ERROR_SUCCESS:
        return QStringLiteral("MPV_ERROR_SUCCESS");
    case MPV_ERROR_EVENT_QUEUE_FULL:
        return QStringLiteral("MPV_ERROR_EVENT_QUEUE_FULL");
    case MPV_ERROR_NOMEM:
        return QStringLiteral("MPV_ERROR_EVENT_QUEUE_FULL");
    case MPV_ERROR_UNINITIALIZED:
        return QStringLiteral("MPV_ERROR_UNINITIALIZED");
    case MPV_ERROR_INVALID_PARAMETER:
        return QStringLiteral("MPV_ERROR_INVALID_PARAMETER");
    case MPV_ERROR_OPTION_NOT_FOUND:
        return QStringLiteral("MPV_ERROR_OPTION_NOT_FOUND");
    case MPV_ERROR_OPTION_FORMAT:
        return QStringLiteral("MPV_ERROR_OPTION_FORMAT");
    case MPV_ERROR_OPTION_ERROR:
        return QStringLiteral("MPV_ERROR_OPTION_ERROR");
    case MPV_ERROR_PROPERTY_NOT_FOUND:
        return QStringLiteral("MPV_ERROR_PROPERTY_NOT_FOUND");
    case MPV_ERROR_PROPERTY_FORMAT:
        return QStringLiteral("MPV_ERROR_PROPERTY_FORMAT");
    case MPV_ERROR_PROPERTY_UNAVAILABLE:
        return QStringLiteral("MPV_ERROR_PROPERTY_UNAVAILABLE");
    case MPV_ERROR_PROPERTY_ERROR:
        return QStringLiteral("MPV_ERROR_PROPERTY_ERROR");
    case MPV_ERROR_COMMAND:
        return QStringLiteral("MPV_ERROR_COMMAND");
    case MPV_ERROR_LOADING_FAILED:
        return QStringLiteral("MPV_ERROR_LOADING_FAILED");
    case MPV_ERROR_AO_INIT_FAILED:
        return QStringLiteral("MPV_ERROR_AO_INIT_FAILED");
    case MPV_ERROR_VO_INIT_FAILED:
        return QStringLiteral("MPV_ERROR_VO_INIT_FAILED");
    case MPV_ERROR_NOTHING_TO_PLAY:
        return QStringLiteral("MPV_ERROR_NOTHING_TO_PLAY");
    case MPV_ERROR_UNKNOWN_FORMAT:
        return QStringLiteral("MPV_ERROR_UNKNOWN_FORMAT");
    case MPV_ERROR_UNSUPPORTED:
        return QStringLiteral("MPV_ERROR_UNSUPPORTED");
    case MPV_ERROR_NOT_IMPLEMENTED:
        return QStringLiteral("MPV_ERROR_NOT_IMPLEMENTED");
    case MPV_ERROR_GENERIC:
        return QStringLiteral("MPV_ERROR_GENERIC");
    }
    return QString();
}

#include "moc_mpvcontroller.cpp"
