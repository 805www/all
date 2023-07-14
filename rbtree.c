#include <assert.h>
#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

// ========================================================================
// 实现红黑树
// ========================================================================
struct rb_node {
	struct rb_node *rb_parent, *rb_left, *rb_right;
#define RB_RED 0
#define RB_BLACK 1
	unsigned int color;
};

struct rb_root {
	struct rb_node *rb_node;
};

#define container_of(ptr, type, member) \
	((type *)((char *)((type *)(ptr)) - offsetof(type, member)))

#define rb_entry(ptr, type, member) container_of(ptr, type, member)

void rb_node_init(struct rb_node *node)
{
	node->color = RB_RED;
	node->rb_parent = node->rb_left = node->rb_right = NULL;
}

void rb_root_init(struct rb_root *root)
{
	root->rb_node = NULL;
}

void __rb_rotate_left(struct rb_node *node, struct rb_root *root)
{
	struct rb_node *parent = node->rb_parent;
	struct rb_node *gparent = parent->rb_parent;
	struct rb_node *lnode = node->rb_left;

	node->rb_left = parent;
	parent->rb_parent = node;

	parent->rb_right = lnode;
	if (lnode)
		lnode->rb_parent = parent;

	node->rb_parent = gparent;
	if (gparent == NULL)
		root->rb_node = node;
	else if (gparent->rb_left == parent)
		gparent->rb_left = node;
	else
		gparent->rb_right = node;
}

void __rb_rotate_right(struct rb_node *node, struct rb_root *root)
{
	struct rb_node *parent = node->rb_parent;
	struct rb_node *gparent = parent->rb_parent;
	struct rb_node *rnode = node->rb_right;

	node->rb_right = parent;
	parent->rb_parent = node;

	parent->rb_left = rnode;
	if (rnode)
		rnode->rb_parent = parent;

	node->rb_parent = gparent;
	if (gparent == NULL)
		root->rb_node = node;
	else if (gparent->rb_left == parent)
		gparent->rb_left = node;
	else
		gparent->rb_right = node;
}

void rb_insert_color(struct rb_node *node, struct rb_root *root)
{
	struct rb_node *parent;

	while ((parent = node->rb_parent) && parent->color == RB_RED) {
		struct rb_node *gparent = parent->rb_parent;
		assert(gparent);
		assert(gparent->color == RB_BLACK);

		struct rb_node *uncle;

		if (parent == gparent->rb_left) {
			uncle = gparent->rb_right;

			if (!uncle || uncle->color == RB_RED) {
				parent->color = RB_BLACK;
				uncle->color = RB_BLACK;
				gparent->color = RB_RED;
				node = gparent;
				continue;
			}

			if (node == parent->rb_right) {
				struct rb_node *tmp;
				__rb_rotate_left(node, root);
				tmp = parent;
				parent = node;
				node = tmp;
			}

			parent->color = RB_BLACK;
			gparent->color = RB_RED;
			__rb_rotate_right(parent, root);
		} else {
			uncle = gparent->rb_left;

			if (!uncle || uncle->color == RB_RED) {
				parent->color = RB_BLACK;
				uncle->color = RB_BLACK;
				gparent->color = RB_RED;
				node = gparent;
				continue;
			}

			if (node == parent->rb_left) {
				struct rb_node *tmp;
				__rb_rotate_right(node, root);
				tmp = node;
				node = parent;
				parent = tmp;
			}

			parent->color = RB_BLACK;
			gparent->color = RB_RED;
			__rb_rotate_left(parent, root);
		}
	}
	root->rb_node->color = RB_BLACK;
}

void rb_link_node(struct rb_node *node, struct rb_node *parent,
		  struct rb_node **link)
{
	node->rb_parent = parent;
	node->rb_left = node->rb_right = NULL;

	node->color = RB_RED;

	*link = node;
}

#define rb_node_add(root, newnode, compare_fn, duplicate)          \
	do {                                                       \
		int __hr = 0;                                      \
		struct rb_node *__duplicate = NULL;                \
		struct rb_node **__link = &((root)->rb_node);      \
		struct rb_node *__parent = NULL;                   \
		while (__link[0]) {                                \
			__parent = __link[0];                      \
			__hr = (compare_fn)((newnode), __parent);  \
			if (__hr == 0) {                           \
				__duplicate = __parent;            \
				break;                             \
			} else if (__hr < 0) {                     \
				__link = &(__link[0]->rb_left);    \
			} else {                                   \
				__link = &(__link[0]->rb_right);   \
			}                                          \
		}                                                  \
		(duplicate) = __duplicate;                         \
		if (__duplicate == NULL) {                         \
			rb_link_node((newnode), __parent, __link); \
			rb_insert_color((newnode), root);          \
		}                                                  \
	} while (0)

#define rb_node_find(root, what, compare_fn, duplicate)   \
	do {                                              \
		struct rb_node *__p = (root)->rb_node;    \
		(duplicate) = NULL;                       \
		int __hr = 0;                             \
		while (__p) {                             \
			__hr = (compare_fn)((what), __p); \
			if (__hr == 0) {                  \
				(duplicate) = __p;        \
				break;                    \
			} else if (__hr < 0) {            \
				__p = __p->rb_left;       \
			} else {                          \
				__p = __p->rb_right;      \
			}                                 \
		}                                         \
	} while (0)

// ========================================================================
// test
// ========================================================================
struct val {
	int i;
	struct rb_node rbnode;
};

struct val *val_create(int i)
{
	struct val *v = (struct val *)malloc(sizeof(struct val));
	v->i = i;
	rb_node_init(&v->rbnode);
	return v;
}

void val_free(struct val *v)
{
	// rb_erase_node(v);
	free(v);
}

int val_compare(struct rb_node *lhs, struct rb_node *rhs)
{
	struct val *vl = rb_entry(lhs, struct val, rbnode);
	struct val *vr = rb_entry(rhs, struct val, rbnode);
	return vl->i - vr->i;
}

int main()
{
	struct rb_root rbroot;
	rb_root_init(&rbroot);

	for (int i = 0; i < 20000; i++) {
		struct val *v = val_create(i);
		struct rb_node *duplicate = NULL;
		rb_node_add(&rbroot, &v->rbnode, val_compare, duplicate);
		assert(duplicate == NULL);
	}

	assert(0);

	struct val dumy;
	dumy.i = 0;
	rb_node_init(&dumy.rbnode);

	struct rb_node *duplicate = NULL;
	rb_node_find(&rbroot, &dumy.rbnode, val_compare, duplicate);
	assert(duplicate != NULL);
	struct val *find_ans = rb_entry(duplicate, struct val, rbnode);
	printf("find it : %d\n", find_ans->i);
}
