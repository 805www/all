#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

/*
 * 模仿 Linux 内核，实现双向循环链表
 *
 * add
 * delete
 * is_ / empty()
 * replace
 * move
 * splice
 * cut
 * for
 */

struct list_head {
    struct list_head *prev, *next;
};

static inline void list_head_init(struct list_head *head) {
    head->prev = head;
    head->next = head;
}

static inline void __list_add(struct list_head *newnode, struct list_head *prev,
                              struct list_head *next) {
    newnode->prev = prev;
    newnode->next = next;
    prev->next = newnode;
    next->prev = newnode;
}

static inline void list_add(struct list_head *newnode, struct list_head *head) {
    __list_add(newnode, head, head->next);
}

static inline void list_add_tail(struct list_head *newnode,
                                 struct list_head *head) {
    __list_add(newnode, head->prev, head);
}

static inline void __list_del(struct list_head *prev, struct list_head *next) {
    prev->next = next;
    next->prev = prev;
}

static inline void __list_del_entry(struct list_head *entry) {
    __list_del(entry->prev, entry->next);
}

static inline void list_del(struct list_head *entry) {
    __list_del_entry(entry);
    entry->prev = 0;
    entry->next = 0;
}

static inline void list_del_init(struct list_head *entry) {
    __list_del_entry(entry);
    list_head_init(entry);
}

static inline void list_replace(struct list_head *oldnode,
                                struct list_head *newnode) {
    newnode->prev = oldnode->prev;
    newnode->next = oldnode->next;
    oldnode->next->prev = newnode;
    oldnode->prev->next = newnode;
}

static inline void list_replace_init(struct list_head *oldnode,
                                     struct list_head *newnode) {
    list_replace(oldnode, newnode);
    list_head_init(oldnode);
}

static inline bool list_is_first(struct list_head *list,
                                 struct list_head *head) {
    return list->prev = head;
}

static inline bool list_is_last(struct list_head *list,
                                struct list_head *head) {
    return list->next = head;
}

static inline bool list_empty(struct list_head *head) {
    return head->next == head;
}

static inline bool list_is_head(struct list_head *list,
                                struct list_head *head) {
    return list == head;
}

static inline int list_is_singular(const struct list_head *head) {
    return !list_empty(head) && (head->next == head->prev);
}

static inline void __list_splice(struct list_head *list, struct list_head *prev,
                                 struct list_head *next) {
    struct list_head *first = list->next;
    struct list_head *last = list->prev;

    first->prev = prev;
    last->next = next;
    prev->next = first;
    next->prev = last;
}

static inline void list_splice(struct list_head *list, struct list_head *head) {
    if (!list_empty(head)) __list_splice(list, head, head->next);
}

static inline void list_splice_tail(struct list_head *list,
                                    struct list_head *head) {
    if (!list_empty(head)) __list_splice(list, head->prev, head);
}

static inline void list_splice_init(struct list_head *list,
                                    struct list_head *head) {
    if (!list_empty(head)) {
        __list_splice(list, head, head->next);
        list_head_init(head);
    }
}

static inline void list_splice_tail_init(struct list_head *list,
                                         struct list_head *head) {
    if (!list_empty(head)) {
        __list_splice(list, head->prev, head);
        list_head_init(head);
    }
}

static inline void list_move(struct list_head *list, struct list_head *head) {
    __list_del_entry(list);
    list_add(list, head);
}

static inline void list_move_tail(struct list_head *list,
                                  struct list_head *head) {
    __list_del_entry(list);
    list_add_tail(list, head);
}

static inline void __list_cut_postion(struct list_head *list,
                                      struct list_head *head,
                                      struct list_head *entry) {
    struct list_head *first = entry->next;
    list->next = head->next;
    head->next->prev = list->next;
    list->prev = entry;
    entry->next = list;
    head->next = first;
    first->prev = head;
}

static inline void list_cut_postion(struct list_head *list,
                                    struct list_head *head,
                                    struct list_head *entry) {
    if (list_empty(head)) return;
    if (list_is_singular(head) && !list_is_head(entry, head) &&
        (entry != head->next))
        return;
    if (list_is_head(entry, head))
        list_head_init(list);
    else
        __list_cut_postion(list, head, entry);
}

static inline void list_cut_before(struct list_head *list,
                                   struct list_head *head,
                                   struct list_head *entry) {
    if (head->next == entry)
        list_head_init(list);
    else {
        list->next = head->next;
        list->next->prev = list;
        list->prev = entry->prev;
        list->prev->next = list;
        head->next = entry;
        entry->prev = head;
    }
}
