#include <stdbool.h>
#include <stdio.h>

/*
 * 模仿 Linux 内核，实现单链表
 *
 * 批量add / add 头
 * 删除all / del 头
 */

struct llist_node {
    struct llist_node *next;
};

struct llist_head {
    struct llist_node *first;
};

#define LLIST_HEAD_INIT \
    { .first = NULL; }
#define LLIST_HEAD(name) struct llist_head name = LLIST_HEAD_INIT
#define llist_node_init(n) ((n)->next = NULL)

#define container_of(ptr, type, member) \
    ((type *)((char *)((type *)(ptr)) - offsetof(type, member)))
#define llist_entry(ptr, type, member) container_of(ptr, type, member)

static inline bool llist_empty(struct llist_head *h) { return !h->first; }

static inline struct llist_node *llist_next(struct llist_node *n) {
    return n->next;
}

static inline bool llist_add_batch(struct llist_node *first,
                                   struct llist_node *last,
                                   struct llist_head *head) {
    last->next = head->first;
    head->first = first;
    return last->next == NULL;
}

static inline bool llist_add(struct llist_node *n, struct llist_head *h) {
    return llist_add_batch(n, n, h);
}

static inline struct llist_node *llist_del_first(struct llist_head *h) {
    struct llist_node *first = h->first;
    if (first) h->first = first->next;
    return first;
}

static inline struct llist_node *llist_del_all(struct llist_head *h) {
    struct llist_node *first = h->first;
    h->first = NULL;
    return first;
}

static inline struct llist_node *llist_reverse_order(struct llist_head *h) {
    struct llist_node *prev = NULL;
    struct llist_node *curr = h->first;

    while (curr) {
        struct llist_node *next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }

    h->first = prev;
    return h->first;
}

#define llist_for_each(pos, head) \
    for ((pos) = (head)->first; (pos); (pos) = (pos)->next)

#define llist_for_each_safe(pos, n, head)            \
    for ((pos) = (head)->first; (pos) && ({          \
                                    n = (pos)->next; \
                                    1;               \
                                });                  \
         (pos) = (n))

#define llist_for_each_entry(pos, head, member)                             \
    for ((pos) = llist_entry((head)->first, typeof(*(pos)), member); (pos); \
         (pos) = llist_entry((pos)->member.next, typeof(*(pos)), member))

#define llist_for_each_entry_safe(pos, n, head, member)              \
    for ((pos) = llist_entry((head)->first, typeof(*(pos)), member); \
         (pos) && ({                                                 \
             (n) = (pos)->member.next;                               \
             1;                                                      \
         })(pos) = llist_entry((n), typeof(*(pos)), member))
