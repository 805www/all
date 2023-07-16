#include <assert.h>
#include <locale.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

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

#define container_of(ptr, type, member) \
    ((type *)((char *)((type *)(ptr)) - offsetof(type, member)))
#define list_entry(ptr, type, member) container_of(ptr, type, member)

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
    return list->prev == head;
}

static inline bool list_is_last(struct list_head *list,
                                struct list_head *head) {
    return list->next == head;
}

static inline bool list_empty(struct list_head *head) {
    return head->next == head;
}

static inline bool list_is_head(struct list_head *list,
                                struct list_head *head) {
    return list == head;
}

// static inline int list_is_singular(const struct list_head *head) {
//     return !list_empty(head) && (head->next == head->prev);
// }

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

// static inline void list_cut_postion(struct list_head *list,
//                                     struct list_head *head,
//                                     struct list_head *entry) {
//     if (list_empty(head)) return;
//     if (list_is_singular(head) && !list_is_head(entry, head) &&
//         (entry != head->next))
//         return;
//     if (list_is_head(entry, head))
//         list_head_init(list);
//     else
//         __list_cut_postion(list, head, entry);
// }

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

#define list_for_each(pos, head) \
    for ((pos) = (head)->next; (pos) != (head); (pos) = (pos)->next)

#define list_for_each_entry(pos, head, member)                     \
    for ((pos) = list_entry((head)->next, typeof(*(pos)), member); \
         &((pos)->member) != (head);                               \
         (pos) = list_entry((pos)->member.next, typeof(*(pos)), member))

/*
 * 使用设计的链表解具体的算法题目
 */

// 题目描述（HDU1276）：某部队进行新兵队列训练，将新兵从一开始按顺序依次编号，并排成一行横队。
// 训练的规则为从头开始进行1至2报数，凡报2的出列，剩下的向小序号方向靠拢，再从头开始进行1至3报数，
// 凡报到3的出列，剩下的向小序号方向靠拢，
// 继续从头开始进行1至2报数……以后从头开始轮流进行1至2报数、1至3报数，
// 直到剩下的人数不超过3人时为止。
//
// 输入：包含多个测试用例，第1行为测试用例数N，接着为N行新兵人数（不超过5000）。
//
// 输出：单行输出剩下的新兵的最初编号，编号之间有一个空格。
//

struct solar {
    int id;
    struct list_head head;
};

#define solar_init(s, i)              \
    do {                              \
        (s)->id = (i);                \
        list_head_init(&((s)->head)); \
    } while (0)

void resolve_solar() {
    int n;
    scanf("%d", &n);

    struct list_head head;
    list_head_init(&head);

    struct solar *arr = malloc(sizeof(struct solar) * n);

    for (int i = 1; i <= n; i++) {
        solar_init(&arr[i], i);
        list_add_tail(&arr[i].head, &head);
    }

    // 计算 size 的数量
    int done = 0;
    int h = 2;
    int sz = n;
    while (!done) {
        struct list_head *curr = head.next;
        int k = 1;
        while (curr != &head) {
            if (k == h) {
                k = 1;
                curr = curr->next;
                list_del_init(curr->prev);
                --sz;
                if (sz == 3) {
                    done = 1;
                    break;
                }
            } else {
                ++k;
                curr = curr->next;
            }
        }
        h == 2 ? (h = 3) : (h = 2);
    }

    struct solar *pos;
    list_for_each_entry(pos, &head, head) { printf("%d ", pos->id); }
    printf("\n\n");
}

int main() { resolve_solar(); }
