#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

/*
 * 模仿 Linux 内核，实现侵入式哈希表
 *
 */

struct hlist_node {
    struct hlist_node *next;
    struct hlist_node **pprev;
};

struct hlist_head {
    struct hlist_node *first;
};

#define container_of(ptr, type, member) \
    ((type *)((char *)((type *)(ptr)) - offsetof(type, member)))
#define hlist_entry(ptr, type, member) container_of(ptr, type, member)

#define HLIST_HEAD_INIT \
    { .first = NULL }
#define HLIST_HEAD(name) struct hlist_head name = HLIST_HEAD_INIT
#define hlist_head_init(h) ((h)->first = NULL)
#define hlist_node_init(n) ((n)->pprev = 0, (n)->next = 0)

static inline int hlist_empty(struct hlist_head *h) { return h->first == NULL; }

static inline int hlist_unhashed(struct hlist_node *e) {
    return e->pprev == NULL;
}

static inline void hlist_add_head(struct hlist_node *n, struct hlist_head *h) {
    n->next = h->first;
    n->pprev = &h->first;
    if (h->first) h->first->pprev = &n->next;
    h->first = n;
}

static inline void hlist_add_before(struct hlist_node *n,
                                    struct hlist_node *next) {
    n->next = next;
    n->pprev = next->pprev;
    *next->pprev = n;
    next->pprev = &n->next;
}

static inline void hlist_add_behind(struct hlist_node *n,
                                    struct hlist_node *prev) {
    n->next = prev->next;
    n->pprev = &prev->next;
    if (prev->next) prev->next->pprev = &(n->next);
    prev->next = n;
}

static inline void __hlist_del(struct hlist_node *n) {
    *(n->pprev) = n->next;
    if (n->next) n->next->pprev = n->pprev;
}

static inline void hlist_del(struct hlist_node *n) {
    __hlist_del(n);
    n->pprev = 0;
    n->next = 0;
}

static inline void hlist_del_init(struct hlist_node *n) {
    __hlist_del(n);
    hlist_node_init(n);
}

static inline bool hlist_is_singular_node(struct hlist_node *n,
                                          struct hlist_head *h) {
    return n->next == NULL && n->pprev == &(h->first);
}

#define hlist_entry_safe(ptr, type, member)                  \
    ({                                                       \
        typeof(ptr) ____ptr = (ptr);                         \
        ____ptr ? hlist_entry(____ptr, type, member) : NULL; \
    })

#define hlist_for_each(pos, head) \
    for ((pos) = (head)->first; (pos); (pos) = (pos)->next)

#define hlist_for_each_entry(pos, head, member)                           \
    for ((pos) = hlist_entry_safe((head)->first, typeof(*(pos)), member); \
         (pos);                                                           \
         (pos) = hlist_entry_safe((pos)->member.next, typeof(*(pos)), member))

#define hlist_for_each_entry_from(pos, head, member) \
    for (; (pos);                                    \
         (pos) = hlist_entry_safe((pos)->member.next, typeof(*(pos)), member))

#define hlist_for_each_entry_continue(pos, head, member)                       \
    for ((pos) = hlist_entry_safe((pos)->member.next, typeof(*(pos)), member); \
         (pos);                                                                \
         (pos) = hlist_entry_safe((pos)->member.next, typeof(*(pos)), member))

#define hlist_for_each_safe(pos, n, head)            \
    for ((pos) = (head)->first; (pos) && ({          \
                                    n = (pos)->next; \
                                    1;               \
                                });                  \
         (pos) = n)

#define hlist_for_each_entry_safe(pos, n, head, member)              \
    for ((pos) = hlist_entry((head)->first, typeof(*(pos)), member); \
         (pos) && ({                                                 \
             n = (pos)->member.next;                                 \
             1;                                                      \
         });                                                         \
         (pos) = hlist_entry(n, typeof(*(pos)), member))

int main() {}
