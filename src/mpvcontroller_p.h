#ifndef MPVCONTROLLER_P_H_INCLUDED
#define MPVCONTROLLER_P_H_INCLUDED

class MpvControllerPrivate
{
public:
    MpvControllerPrivate(MpvController *q);
    MpvController *q_ptr;

    mpv_node_list *create_list(mpv_node *dst, bool is_map, int num);
    void setNode(mpv_node *dst, const QVariant &src);
    bool test_type(const QVariant &v, QMetaType::Type t);
    void free_node(mpv_node *dst);
    QVariant node_to_variant(const mpv_node *node);

    mpv_handle *m_mpv{nullptr};
};

#endif // MPVCONTROLLER_P_H_INCLUDED
