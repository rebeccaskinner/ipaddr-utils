/* Copyright Tim Skinner 2012
 * This source code is covered under the GNU Lesser General Public License.
 * See the LICENSE file for the complete text of the license covering this
 * file.
 */
#include "shm_queue.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define ROUND_TO_PAGESIZE(s) ({ \
        off_t __sz = (s); \
        int   __pagesize = getpagesize(); \
        (__sz / __pagesize) + (__pagesize * (!!(__sz & (__pagesize - 1)))); \
})

typedef unsigned char byte;

struct _shm_queue
{
    uint32_t          size_in;
    uint32_t          size_out;
    uint32_t          allocated;
    volatile off_t    in_ptr;
    volatile off_t    out_ptr;
    volatile off_t    barrier;
    byte     data[];
};

typedef struct _shm_queue _shm_queue_t;

static _shm_queue_t* _shm_queue_create(const char* shm_path,
                                       size_t      shm_size,
                                       int*        shm_fd)
{
    struct _shm_queue* q;
    struct stat st;
    off_t mmap_size = ROUND_TO_PAGESIZE(shm_size + sizeof(struct _shm_queue));
    int fd = shm_open(shm_path,
                      O_CREAT | O_RDWR,
                      S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if(-1 == (*shm_fd = fd))
        return NULL;

    fstat(fd, &st);
    if(0 == st.st_size)
        ftruncate(fd,mmap_size);
    else
        mmap_size = st.st_size;

    q = mmap(0,mmap_size,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    if(MAP_FAILED == q)
    {
        printf("mmap failed: %s\n",strerror(errno));
        return NULL;
    }

    q->allocated = mmap_size;
    return q;
}

static int _shm_write(_shm_queue_t* q, byte* data, size_t len)
{
    return 0;
}

shm_queue_t* shm_queue_new(const char* path, uint32_t* size)
{
    shm_queue_t* q = malloc(sizeof(shm_queue_t));
    int shm_fd = 0;
    size_t shm_size = size?*size:(size_t)getpagesize();
    q->queue = _shm_queue_create(path,shm_size,&shm_fd);
    if(!q->queue) goto error;
    *size = q->queue->allocated;
error:
    free(q);
    return NULL;
}

uint32_t shm_queue_allocated(shm_queue_t* q)
{
    return q->queue->allocated;
}

void shm_queue_free(shm_queue_t* q)
{
    if(!q) return;
    if(q->queue)
        munmap(q->queue, q->queue->allocated);
    free(q);
}
