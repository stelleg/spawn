#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <setjmp.h>

typedef void* arg;
typedef void (*fun)(arg a);
typedef unsigned char bool;
bool true = 1;
bool false = 0;

uint64_t donebit = 0x800000000000000;
uint64_t isdone(uint64_t x){return x & donebit;}
uint64_t done(uint64_t x){return x | donebit;}
uint64_t notdone(uint64_t x){return x & ~donebit;}
uint64_t ind(uint64_t x){return notdone(x);}

// Closure type
typedef struct {
  fun f;
  arg a;
  bool* ret;
} closure; 

// Maybe closure
typedef struct {
  bool b;
  closure c;
} maybe_closure;

// Lifo queue (stack): enqueue to head, dequeue from head.
typedef struct {
  closure* q;
  uint64_t head;
  size_t size;
} queue;

typedef struct {
  uint32_t tid;
  queue* q;
} worker_data;
 
queue alloc_queue(size_t size){
  closure* c = malloc(sizeof(closure) * size);
  return (queue) {c, done(0), size};
}

bool enqueue(closure c, queue* q){
  while(true){
    uint64_t head = q->head;
    if(ind(head) >= q->size) return false;
    if(isdone(head)){
      if(!__sync_bool_compare_and_swap(&q->head, head, notdone(head+1))) continue;
      q->q[ind(q->head-1)] = c;
      q->head = done(q->head);
      return true;
    }
  }
}

maybe_closure dequeue(queue* q){
  uint64_t head = q->head;
  closure c;
  if(ind(head) > 0 && isdone(head)){
    if(!__sync_bool_compare_and_swap(&q->head, head, notdone(head-1))) 
      return (maybe_closure) {false, c};
    c = q->q[ind(q->head)];
    q->head = done(q->head);
    return (maybe_closure) {true, c};
  }   
  return (maybe_closure) {false, c};
} 

void call_closure(closure c){
  fun f = c.f;
  arg a = c.a;
  f(a);
  if(c.ret) *c.ret = true;
}

void spawn(fun f, arg* a, bool* ret, queue* q){
  closure c = (closure) {f, a, ret}; 
  if(!enqueue(c, q)) call_closure(c); 
}

void getwork(queue* q){
  maybe_closure mc = dequeue(q);
  if(mc.b) call_closure(mc.c);
}

void waiton(bool* ret, queue* q){
  while(!*ret) getwork(q);
}

void worker(void *q){
  while(true) getwork((queue*)q);
}

void create_workers(uint32_t num_threads, queue *q){
  pthread_t ps[num_threads];
  for(uint32_t i=0; i<num_threads; i++){
    pthread_create(ps+i, NULL, (void *) &worker, q);
  }
}

void yield(queue* q){
  while(ind(q->head)) getwork(q);
}