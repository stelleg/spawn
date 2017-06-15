#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// Parameters 
typedef struct {
  void* r0;
  void* r1;
  void* r2;
  void* r3;
  void* r4;
  void* r5;
  void* r6;
} arg;

//Function
typedef void (*fun)(arg a);

// Closure 
typedef struct {
  fun f;
  arg a;
} closure; 

// Maybe closure
typedef struct {
  bool b;
  closure c;
} maybe_closure;

// Lifo queue (stack): enqueue to head, dequeue from head.
typedef struct {
  closure* q;
  volatile uint64_t head;
  size_t size;
} queue;

typedef struct {
  uint32_t tid;
  queue* q;
} worker_data;

queue global_queue;
 
queue alloc_queue(size_t size);

bool enqueue(closure c, queue* q);

maybe_closure dequeue(queue* q);

void call_closure(closure c);

void spawn(fun f, arg a);

void spawn_to(fun f, arg a, queue* q);

void getwork_from(queue* q);

void getwork();

void worker(void *q);

void create_workers(uint32_t num_threads, queue *q);

void flush(queue* q);

void init(size_t nworkers, size_t queue_size);

