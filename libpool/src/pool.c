#include <ctype.h>
#include <cwmp/pool.h>

#ifndef WIN32
#include <stdint.h>
#include <unistd.h>
#endif



#define POOL_ALIGNMENT sizeof(unsigned long)
#define pool_align(d, a)     (((d) + (a - 1)) & ~(a - 1))
#define pool_align_ptr(p, a)                                                   \
	(unsigned char *) (((unsigned long) (p) + ((unsigned int) a - 1)) & ~((unsigned long) a - 1))

#ifdef WIN32
#define close_fd    closesocket
#else
#define close_fd    close
#endif



struct pool_cleanup_t
{
	pool_cleanup_handler   handler;
	void                 *data;
	pool_cleanup_t   *next;
};




struct pool_large_t
{
	pool_large_t     *next;
	void                 *alloc;
};


struct pool_data_t
{
	unsigned char               *last;
	unsigned char               *end;
	pool_t           *next;
} ;


struct pool_t
{
	pool_data_t       d;
	size_t			size;
	size_t                max;
	pool_t           *current;
	pool_chain_t          *chain;
	pool_large_t     *large;
	pool_cleanup_t   *cleanup;
};



struct pool_cleanup_file_t
{
	int              fd;
	unsigned char               *name;
} ;




static void *pool_palloc_block(pool_t *pool, size_t size);
static void *pool_palloc_large(pool_t *pool, size_t size);



static void * pool_alloc(size_t size)
{
	void  *p;
	//    cwmp_log_debug("------------------pool alloc size: %d------------------\n", size);
	p = malloc(size);

	if (p == NULL)
	{
		//        cwmp_log_error("malloc() %d bytes failed", size);
	}

	return p;
}



#if (POOLITE_HAVE_POSIX_MEMALIGN)

void * pool_memalign(size_t alignment, size_t size)
{
	void  *p;

	if (posix_memalign(&p, alignment, size) == -1)
	{
		//        cwmp_log_error("posix_memalign() %uz bytes aligned to %uz failed",
		size, alignment);
	}
	return p;
}

#elif (POOLITE_HAVE_MEMALIGN)

static void * pool_memalign(size_t alignment, size_t size)
{
	void  *p;

	//p = memalign(alignment, size);
	if (p == NULL)
	{
		//        cwmp_log_error("memalign() %uz bytes aligned to %uz failed",
		size, alignment);
	}


	return p;
}

#endif


pool_t * pool_create(size_t size)
{
	pool_t  *p;

	//FUNCTION_TRACE();

	p = pool_alloc(size);
	if (p == NULL)
	{
		return NULL;
	}

	p->d.last = (unsigned char *) p + sizeof(pool_t);
	p->d.end = (unsigned char *) p + size;
	p->d.next = NULL;

	p->size = size;

	size = size - sizeof(pool_t);
	p->max = (size < POOL_MAX_ALLOC_FROM_POOL) ? size : POOL_MAX_ALLOC_FROM_POOL;


	p->current = p;
	p->chain = NULL;
	p->large = NULL;
	p->cleanup = NULL;

	return p;
}


void pool_destroy(pool_t *pool)
{
	pool_t          *p, *n;
	pool_large_t    *l;
	pool_cleanup_t  *c;
	//FUNCTION_TRACE();
	if(!pool)
	{
		return;
	}
	for (c = pool->cleanup; c; c = c->next)
	{
		if (c->handler)
		{

			c->handler(c->data);
		}
	}

	for (l = pool->large; l; l = l->next)
	{

		if (l->alloc)
		{
			//            cwmp_log_debug("destroy large alloc %p\n", l->alloc);
			FREE(l->alloc);
		}
	}

#if (POOL_DEBUG)

	/*
	 * we could allocate the pool->log from this pool
	 * so we can not use this log while the free()ing the pool
	 */

	for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next)
	{


		if (n == NULL)
		{
			break;
		}
	}

#endif

	for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next)
	{
		FREE(p);

		if (n == NULL)
		{
			break;
		}
	}
}

void pool_clear(pool_t *pool)
{
	pool_t          *p, *n;
	pool_large_t    *l;
	pool_cleanup_t  *c;
	//FUNCTION_TRACE();

	for (c = pool->cleanup; c; c = c->next)
	{
		if (c->handler)
		{

			c->handler(c->data);
		}
	}

	for (l = pool->large; l; l = l->next)
	{
		//        cwmp_log_debug("clear large alloc %p\n", l->alloc);
		if (l->alloc)
		{
			FREE(l->alloc);
		}
	}

	for (p = pool->d.next; p; p = n, n = n->d.next)
	{
		n = p->d.next;
		//p->d.last = (unsigned char *)p + sizeof(pool_data_t);
		FREE(p);
		if (n == NULL)
		{
			break;
		}
	}

	pool->d.last = (unsigned char *) pool + sizeof(pool_t);
	pool->d.next = NULL;

	pool->current = pool;
	pool->chain = NULL;
	pool->large = NULL;
	pool->cleanup = NULL;

}


void * pool_palloc(pool_t *pool, size_t size)
{
	unsigned char      *m;
	pool_t  *p;

	if (size <= pool->max)
	{

		//cwmp_log_debug("palloc size(%d)/max(%d)\n", size, pool->max);
		p = pool->current;

		do
		{
			m = pool_align_ptr(p->d.last, POOL_ALIGNMENT);

			if ((size_t) (p->d.end - m) >= size)
			{
				p->d.last = m + size;

				return m;
			}

			p = p->d.next;

		}
		while (p);

		return pool_palloc_block(pool, size);
	}

	return pool_palloc_large(pool, size);
}


void * pool_pnalloc(pool_t *pool, size_t size)
{
	unsigned char      *m;
	pool_t  *p;

	if (size <= pool->max)
	{

		p = pool->current;

		do
		{
			m = p->d.last;

			if ((size_t) (p->d.end - m) >= size)
			{
				p->d.last = m + size;

				return m;
			}

			p = p->d.next;

		}
		while (p);

		return pool_palloc_block(pool, size);
	}

	return pool_palloc_large(pool, size);
}


static void * pool_palloc_block(pool_t *pool, size_t size)
{
	unsigned char      *m;
	size_t       psize;
	pool_t  *p, *newp, *current;

	psize = (size_t) (pool->d.end - (unsigned char *) pool);

	m = pool_alloc(psize);
	if (m == NULL)
	{
		return NULL;
	}

	newp = (pool_t *) m;

	newp->d.end = m + psize;
	newp->d.next = NULL;

	m += sizeof(pool_data_t);
	newp->d.last = m + size;

	current = pool->current;

	for (p = current; p->d.next; p = p->d.next)
	{
		if ((size_t) (p->d.end - p->d.last) < POOL_ALIGNMENT)
		{
			current = p->d.next;
		}
	}

	p->d.next = newp;

	pool->current = current ? current : newp;

	return m;
}


static void * pool_palloc_large(pool_t *pool, size_t size)
{
	void              *p;
	pool_large_t  *large;
	//FUNCTION_TRACE();
	//cwmp_log_debug("alloc large memory: %d\n", size);
	p = pool_alloc(size);
	if (p == NULL)
	{
		return NULL;
	}

	large = pool_palloc(pool, sizeof(pool_large_t));
	if (large == NULL)
	{
		//        cwmp_log_debug("large is null, free p %p\n", p);
		FREE(p);
		return NULL;
	}

	large->alloc = p;
	large->next = pool->large;
	pool->large = large;

	return p;
}


	void *
pool_pmemalign(pool_t *pool, size_t size, size_t alignment)
{
	void              *p;
	pool_large_t  *large;

	/*p = pool_memalign(alignment, size);
	  if (p == NULL) {
	  return NULL;
	  }
	 */
	//FUNCTION_TRACE();

	large = pool_palloc(pool, sizeof(pool_large_t));
	if (large == NULL)
	{
		FREE(p);
		return NULL;
	}

	large->alloc = p;
	large->next = pool->large;
	pool->large = large;

	return p;
}


	int
pool_pfree(pool_t *pool, void *p)
{
	pool_large_t  *l;

	for (l = pool->large; l; l = l->next)
	{
		if (p == l->alloc)
		{

			FREE(l->alloc);
			l->alloc = NULL;

			return 0;
		}
	}

	return -1;
}


	void *
pool_pcalloc(pool_t *pool, size_t size)
{
	void *p;

	p = pool_palloc(pool, size);
	if (p)
	{
		memset(p,0, size);
	}

	return p;
}


	pool_cleanup_t *
pool_cleanup_palloc(pool_t *p, size_t size)
{
	pool_cleanup_t  *c;

	c = pool_palloc(p, sizeof(pool_cleanup_t));
	if (c == NULL)
	{
		return NULL;
	}

	if (size)
	{
		c->data = pool_palloc(p, size);
		if (c->data == NULL)
		{
			return NULL;
		}

	}
	else
	{
		c->data = NULL;
	}

	c->handler = NULL;
	c->next = p->cleanup;

	p->cleanup = c;



	return c;
}

void * pool_cleanup_add(pool_t * pool, pool_cleanup_handler handler, void * data)
{
	pool_cleanup_t  *c;

	c = pool_palloc(pool, sizeof(pool_cleanup_t));
	if (c == NULL)
	{
		return NULL;
	}

	c->handler = handler;
	c->data = data;
	c->next = pool->cleanup;

	pool->cleanup = c;

	return c;
}

	void
pool_cleanup_file(void *data)
{
	pool_cleanup_file_t  *c = data;



	if (close_fd(c->fd) == -1)
	{
		//        cwmp_log_error("close() \"%s\" failed", c->name);
	}
}


	void
pool_delete_file(void *data)
{
	pool_cleanup_file_t  *c = data;

	int  err;



	if (unlink(c->name) == -1)
	{
		err = errno;

		if (err != ENOENT)
		{
			//            cwmp_log_error("delete() \"%s\" failed", c->name);
		}
	}

	if (close_fd(c->fd) == -1)
	{
		//        cwmp_log_error("close() \"%s\" failed", c->name);
	}
}


#if 0

	static void *
pool_get_cached_block(size_t size)
{
	void                     *p;
	pool_cached_block_slot_t  *slot;

	if (pool_cycle->cache == NULL)
	{
		return NULL;
	}

	slot = &pool_cycle->cache[(size + pool_pagesize - 1) / pool_pagesize];

	slot->tries++;

	if (slot->number)
	{
		p = slot->block;
		slot->block = slot->block->next;
		slot->number--;
		return p;
	}

	return NULL;
}


#endif


void * pool_prealloc(pool_t * pool, void * ptr, size_t oldsize, size_t newsize)
{
	void * p = pool_pcalloc(pool, newsize);
	if (!p)
	{
		return NULL;
	}
	memcpy(p, ptr, oldsize);
	pool_pfree(pool, ptr);
	return p;
}

void * pool_pmemdup(pool_t * pool, const  void * ptr, size_t size)
{

	void * p = pool_pcalloc(pool, size);
	if (!p)
	{
		return NULL;
	}
	memcpy(p, ptr, size);
	return p;
}


char * pool_pstrdup(pool_t * pool, const  void * ptr)
{
	size_t s;
	if (!ptr || ptr == "")
	{
		return NULL;
	}
	s = strlen(ptr);
	return (char *)pool_pmemdup(pool, ptr, s+1);

}

char * pool_pstrdup_lower (pool_t *pool, const char *ptr)
{
	char *copy = pool_pstrdup (pool, ptr);
	char *p = copy;

	//FUNCTION_TRACE();
	for (; *p; p++)
		*p = tolower (*p);
	return copy;
}


