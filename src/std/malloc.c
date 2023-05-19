/*
 * MIT License
 *
 * Copyright (c) 2017 Embedded Artistry
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "list.h"
#include "printk.h"
#include <assert.h>
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* align to nearest power of two */
#define ALIGN_SIZE(sz, align) (((sz) + ((align)-1)) & ~((align)-1))

/* free list node*/
typedef struct alloc_node {
    struct list_head node;
    size_t size;
    char *block;
} alloc_node_t;

/* allocation metadata size */
#define ALLOC_HEADER_SZ __builtin_offsetof(alloc_node_t, block)

/* minimum allocation size of 32 bytes */
#define MIN_ALLOC_SZ ALLOC_HEADER_SZ + 32

/* free list */
LIST_HEAD(free_list);

static void coalesce_free_list(void) {
    alloc_node_t *b, *lb = NULL, *t;

    list_for_each_entry_safe(b, t, &free_list, node) {
        if (lb) {
            /* coalesce adjacent blocks */
            if ((((uintptr_t)&lb->block) + lb->size) == (uintptr_t)b) {
                lb->size += sizeof(*b) + b->size;
                list_del(&b->node);
                continue;
            }
        }
        lb = b;
    }
}

uint8_t *base;
size_t base_res_size;

void *malloc_unsafe_inc(size_t size) {
    size += 8 - size % 8;
    // printk("%d, res = %d\n", size, base_res_size);

    if (size > base_res_size) {
        return NULL;
    }

    void *ptr = base;
    base += size;
    base_res_size -= size;

    return ptr;
}

void *malloc_unsafe(size_t size) {
    void *ptr = NULL;
    alloc_node_t *blk = NULL;

    if (size > 0) {
        /* Align the pointer */
        size = ALIGN_SIZE(size, sizeof(void *));

        /* try to find a big enough block */
        list_for_each_entry(blk, &free_list, node) {
            if (blk->size >= size) {
                ptr = &blk->block;
                break;
            }
        }

        if (ptr) {
            /* split block if possible */
            if ((blk->size - size) >= MIN_ALLOC_SZ) {
                alloc_node_t *new_blk;
                new_blk = (alloc_node_t *)((uintptr_t)(&blk->block) + size);
                new_blk->size = blk->size - size - ALLOC_HEADER_SZ;
                blk->size = size;
                list_add(&new_blk->node, &blk->node);
            }

            list_del(&blk->node);
        }
    }
    return ptr;
}

void free_unsafe(void *ptr) {
    alloc_node_t *blk, *free_blk;

    if (ptr) {
        blk = container_of(ptr, alloc_node_t, block);

        /* add block to free list in ascending order by pointer */
        list_for_each_entry(free_blk, &free_list, node) {
            if (free_blk > blk) {
                list_add_tail(&blk->node, &free_blk->node);
                goto blockadded;
            }
        }
        list_add_tail(&blk->node, &free_list);

    blockadded:
        coalesce_free_list();
    }
}

void _malloc_addblock(void *addr, size_t size) {
    base = addr;
    base_res_size = size;

    alloc_node_t *blk;

    /* pointer align the block */
    blk = (alloc_node_t *)ALIGN_SIZE((uintptr_t)addr, sizeof(void *));

    /* calculate usable size */
    blk->size = (uintptr_t)addr + size - (uintptr_t)blk - ALLOC_HEADER_SZ;

    /* add the block to the free list */
    list_add(&blk->node, &free_list);
}

void *realloc(void *ptr, size_t size) {
    void *new_data = NULL;

    if (size) {
        if (!ptr) {
            return malloc(size);
        }

        new_data = malloc(size);
        if (new_data) {
            memcpy(new_data, ptr, size); // TODO: unsafe copy...
            free(ptr);                   // we always move the data. free.
        }
    }

    return new_data;
}

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
volatile int cpt = 0;

static uint64_t malloc0;

void *malloc(size_t size) {
    if (size == 0) {
        return &malloc0;
    }

    pthread_mutex_lock(&mutex);
    // printk(" enter #%d\n", size);
    assert(cpt == 0);
    cpt = cpt + 1;

    // void *ptr = malloc_unsafe(size);
    void *ptr = malloc_unsafe_inc(size);

    assert(cpt == 1);
    cpt = cpt - 1;
    // printk("%x leave \n", ptr);
    // printf("size %d -> %x\n", size, ptr);
    pthread_mutex_unlock(&mutex);
    if (!ptr) {
        panic("** OUT OF MEMORY ***\n");
    }
    return ptr;
}

void free(void *ptr) {
    pthread_mutex_lock(&mutex);
    // printk("%x enter\n", ptr);
    assert(cpt == 0);
    cpt = cpt + 1;

    // PR TODO: Bugged ...
    // free_unsafe(ptr);

    assert(cpt == 1);
    cpt = cpt - 1;
    // printk("%x leave\n", ptr);
    pthread_mutex_unlock(&mutex);
}

void *calloc(size_t nmemb, size_t size) {
    size_t n = nmemb * size;
    void *ret = malloc(n);
    if (!ret) {
        return NULL;
    }
    memset(ret, 0, n);
    return ret;
}