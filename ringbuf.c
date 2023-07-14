#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ===========================================================
// ringbuffer
// ===========================================================
typedef ptrdiff_t ilong;
typedef size_t iulong;

struct ringbuf {
	char *data;
	ilong in;
	ilong out;
	ilong size;
};

#define ringbuf_used_size(r)                            \
	(((r)->in >= (r)->out) ? ((r)->in - (r)->out) : \
				 ((r)->size - ((r)->out - (r)->in)))

#define ringbuf_free_size(r) ((r)->size - ringbuf_used_size(r) - 1)

#define ringbuf_empty(r) (ringbuf_used_size(r) == 0)

extern void ringbuf_init(struct ringbuf *r, ilong size);
extern void ringbuf_destory(struct ringbuf *r);

extern ilong ringbuf_write(struct ringbuf *r, const char *data, ilong sz);
extern ilong ringbuf_read(struct ringbuf *r, char *data, ilong sz);
extern ilong ringbuf_peek(struct ringbuf *r, char *data, ilong sz);

void ringbuf_init(struct ringbuf *r, ilong size)
{
	r->data = (char *)malloc(size);
	assert(r->data);
	r->size = size;
	r->in = 0;
	r->out = 0;
}

void ringbuf_destory(struct ringbuf *r)
{
	if (r->data)
		free(r->data);
	r->in = 0;
	r->out = 0;
	r->size = 0;
}

ilong ringbuf_write(struct ringbuf *r, const char *data, ilong sz)
{
	ilong freesz = ringbuf_free_size(r);
	sz = freesz < sz ? freesz : sz;

	ilong msz = r->size - r->in;

	if (sz <= msz) {
		memcpy(r->data + r->in, data, sz);
	} else {
		memcpy(r->data + r->in, data, msz);
		memcpy(r->data, data + msz, sz - msz);
	}

	r->in += sz;
	if (r->in >= r->size)
		r->in %= r->size;

	return sz;
}

ilong ringbuf_peek(struct ringbuf *r, char *data, ilong sz)
{
	ilong usesz = ringbuf_used_size(r);

	sz = usesz < sz ? usesz : sz;

	ilong msz = r->size - r->out;

	if (sz <= msz) {
		memcpy(data, r->data + r->out, sz);
	} else {
		memcpy(data, r->data + r->out, msz);
		memcpy(data + msz, r->data, sz - msz);
	}

	return sz;
}

ilong ringbuf_read(struct ringbuf *r, char *data, ilong sz)
{
	sz = ringbuf_peek(r, data, sz);
	r->out += sz;
	if (r->out >= r->size)
		r->out %= r->size;
	return sz;
}

// ===========================================================
// test ringbuffer
// ===========================================================

void test()
{
	struct ringbuf r;
	ringbuf_init(&r, sizeof(int) * 1024 + 1);

	for (int i = 0; i < 1024; i++) {
		ilong sz = ringbuf_write(&r, (char *)&i, sizeof(int));
		assert(sz == sizeof(int));
	}

	int i = 0;
	while (!ringbuf_empty(&r)) {
		int ret;
		ilong sz = ringbuf_read(&r, (char *)&ret, sizeof(int));
		assert(sz == sizeof(int));
		assert(i == ret);
		i++;
		printf("%d ", ret);
	}
}

int main()
{
	test();
}
