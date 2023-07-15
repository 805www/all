#include <assert.h>
#include <heapapi.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =========================================================
// 设计单链表
// =========================================================

struct slist_node {
  struct slist_node *next;
};

struct slist_head {
  struct slist_node *first;
};

#define slist_entry(ptr, type, member) \
  ((type *)((char *)((type *)(ptr)) - offsetof(type, member)))

#define slist_head_init(h) ((h)->first = 0)
#define slist_node_init(n) ((n)->next = 0)

#define slist_add_head(h, n) ((n)->next = (h)->first, (h)->first = (n))

#define slist_del(h, n)                  \
  do {                                   \
    struct slist_node *__p = NULL;       \
    struct slist_node *__n = (h)->first; \
    while (__n != (n)) {                 \
      __p = __n;                         \
      __n = __n->next;                   \
    }                                    \
    if (__p == NULL)                     \
      (h)->first = (n)->next;            \
    else                                 \
      __p->next = __n->next;             \
    (n)->next = NULL;                    \
  } while (0)

void slist_reverse(struct slist_head *h) {
  struct slist_node *p = NULL;
  struct slist_node *n = h->first;

  while (n) {
    struct slist_node *save = n->next;
    n->next = p;
    p = n;
    n = save;
  }
  h->first = p;
}

#define slist_add_sort(h, n, compare_fn)    \
  do {                                      \
    struct slist_node *__p = NULL;          \
    struct slist_node *__n = (h)->first;    \
    if (__n == NULL) {                      \
      (n)->next = __n;                      \
      (h)->first = (n);                     \
      break;                                \
    }                                       \
    while (__n) {                           \
      int __hr = (compare_fn)((n), __n);    \
      if (__hr <= 0) {                      \
        if (__p == NULL) {                  \
          (n)->next = __n;                  \
          (h)->first = (n);                 \
        } else {                            \
          __p->next = (n);                  \
          (n)->next = __n;                  \
        }                                   \
        break;                              \
      }                                     \
      __p = __n;                            \
      __n = __n->next;                      \
    }                                       \
    if ((n)->next == NULL) __p->next = (n); \
  } while (0)

// ======================================
// test
// ======================================

struct event {
  int val;
  struct slist_node node;
};

struct event *event_new(int val) {
  struct event *e = (struct event *)malloc(sizeof(struct event));
  e->val = val;
  slist_node_init(&e->node);
  return e;
}

int event_compare(struct slist_node *lhs, struct slist_node *rhs) {
  struct event *el = slist_entry(lhs, struct event, node);
  struct event *er = slist_entry(rhs, struct event, node);
  return el->val - er->val;
}

int main() {
  struct slist_head head;
  slist_head_init(&head);

  int nums[] = {1, 3, 2, 33, 123, 23, 1, 312, 15, 17, 11, 12, 13, 16};
  int sz = sizeof(nums) / sizeof(int);
  for (int i = 0; i < sz; i++) {
    struct event *e = event_new(nums[i]);
    slist_add_sort(&head, &e->node, event_compare);
    // slist_add_head(&head, &e->node);
  }

  slist_reverse(&head);
  struct slist_node *n = head.first;
  int count = 0;
  while (n) {
    struct event *v = slist_entry(n, struct event, node);
    count++;
    printf("%d ", v->val);
    n = n->next;
  }
  assert(count == sz);
  printf("\n\n");
}
