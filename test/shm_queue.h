#ifndef __SHM_QUEUE_H__
#define __SHM_QUEUE_H__

#include <stdint.h>

#define SHM_QUEUE_REALLOC 1

struct _shm_queue;
struct shm_queue;

typedef struct shm_queue shm_queue_t;
typedef struct queue queue_t;

struct shm_queue
{
    int fd;
    struct _shm_queue* queue;
};

shm_queue_t* shm_queue_new(const char* path, uint32_t* size);
uint32_t shm_queue_allocated(shm_queue_t* q);
void shm_queue_free(shm_queue_t* q);

#endif
