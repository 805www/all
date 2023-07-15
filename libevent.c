#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// ===================================================
// Linux 内核的侵入式链表
// ===================================================

struct list_head {
  struct list_head *prev, *next;
};

#define container_of(ptr, type, member) \
  ((type *)((char *)((type *)(ptr)) - offsetof(type, member)))

#define list_entry(ptr, type, member) container_of(ptr, type, member)

#define list_head_init(h) ((h)->prev = (h), (h)->next = (h))

#define list_add_head(n, h)                                       \
  ((n)->prev = (h), (n)->next = (h)->next, (h)->next->prev = (n), \
   (h)->next = (n))

#define list_add_tail(n, h)                                       \
  ((n)->prev = (h)->prev, (n)->next = (h), (h)->prev->next = (n), \
   (h)->prev = (n))

#define list_empty(n) ((n)->next == (n))

#define list_del(n) ((n)->prev->next = (n)->next, (n)->next->prev = (n)->prev)

#define list_del_init(n) \
  do {                   \
    list_del(n);         \
    list_head_init(n);   \
  } while (0)

// ====================================================
// event base
// ====================================================

struct event_base {
  struct list_head *fds;
  int nfds;
  int fds_sz;

  struct list_head active;
  struct poll *poll;
};

extern int event_base_loop(struct event_base *base);

// ====================================================
// poll
// ====================================================

// ====================================================
// event
// ====================================================

typedef void (*callback)(int fd, short event, void *args);

#define EV_READ 0x01
#define EV_WRITE 0x02

struct event {
  struct list_head fd_next;
  struct list_head active;
  struct event_base *base;
  int fd;
  short ev_event;
  callback cb;
  void *args;
};

extern void event_init(struct event *e, struct event_base *base, int fd,
                       short ev_event, callback cb, void *args);

extern void event_destory(struct event *e);

extern int event_add(struct event *e);
extern int event_del(struct event *e);
