
class MpvAbstractItemPrivate
{
public:
    MpvAbstractItemPrivate(MpvAbstractItem *q);
    MpvAbstractItem *q_ptr;

    void observeProperty(const QString &property, mpv_format format, int id = 0);
    void cachePropertyValue(const QString &property, const QVariant &value);

    QThread *m_workerThread{nullptr};
    MpvController *m_mpvController{nullptr};
    mpv_handle *m_mpv{nullptr};
    mpv_render_context *m_mpv_gl{nullptr};
    QMap<QString, QVariant> m_propertiesCache;
};
