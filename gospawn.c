#define _GNU_SOURCE
#include<setjmp.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>

#define DEF_SIZE 1024
#define STACK_SIZE 4096

typedef struct buflist_s {
  jmp_buf b;
  struct buflist_s* next;
} buflist;

typedef struct {
  buflist* ready;
  buflist* free;
  buflist* pool;
} worker;

worker makeworker(size_t poolsize){
  buflist* pool = malloc(sizeof(buflist) * poolsize);
  for(int i=0; i<poolsize-1; i++){
    pool[i].next = pool + (i+1);
  }
  pool[poolsize-1].next = NULL;
  return (worker){NULL, pool, pool};
}

worker def; 

jmp_buf* pushreadybuf(worker* w){
  buflist *bl = w->free;
  w->free = bl->next;
  bl->next = w->ready;
  w->ready = bl;
  return &bl->b;
}

typedef struct {bool full; size_t size; buflist* deps;} channel;
typedef channel* chan;

jmp_buf* pushdepbuf(worker* w, chan c){
  buflist *bl = w->free;
  w->free = bl->next;
  bl->next = c->deps;
  c->deps = bl;
  return &bl->b;
}

inline void newstack(){
  void* sp = malloc(STACK_SIZE * sizeof(void*)) + STACK_SIZE; 
  __asm ("mov %0, %%rsp" : "=r" (sp) : : "%rsp");  
}

inline void getwork(worker *w){
  while(1){
    if(w->ready) {
      buflist cur = *w->ready;
      buflist* tmp = w->ready->next;
      w->ready->next = w->free;
      w->free = w->ready;
      w->ready = tmp;
      siglongjmp(cur.b, 1);
    }
  }
}

#define mkcont(fn, bufptr, val) do { \
  if(sigsetjmp(*bufptr, 0) == val){ \
    newstack(); \
    fn; \
    getwork(&def); \
  } \
} while(0);
#define spawn(fn) mkcont(fn, pushreadybuf(&def), 1)
#define go(fn) mkcont(fn, pushreadybuf(&def), 0)
#define spawndep(fn, ch) mkcont(fn, pushdepbuf(&def, ch), 1)
#define godep(fn, ch) mkcont(fn, pushdepbuf(&def, ch), 0)

void chan_send(void* data, chan c){
  if(!c->full){
    *(void**)(((char*)c) + sizeof(channel)) = data;
    while(c->deps != NULL){
      buflist* tmp = c->deps;
      c->deps = tmp->next;
      tmp->next = def.ready;
      def.ready = tmp;
    }
    c->full = true;
  } else {
    spawndep(chan_send(data, c), c);
    getwork(&def);
  }
}

void *chan_rcv(chan c){
  if(c->full){
    while(c->deps != NULL){
      buflist* tmp = c->deps;
      c->deps = tmp->next;
      tmp->next = def.ready;
      def.ready = tmp;
    }
    c->full = false;
    return (void*)(((char*)c) + sizeof(channel));
  } else {
    spawndep(chan_rcv(c), c);
    getwork(&def);
  }
}

chan make_chan(size_t s){
  chan c = (chan)malloc(sizeof(channel) + s);
  *c = (channel){false, s, NULL}; 
  return c;
}

#define mkchan(type) make_chan(sizeof(type))
#define chs(data, c) chan_send((void*) data, c)
#define chr(type, c) *(type*)chan_rcv(c)

void putstrln(char* str, chan c){
  printf(str);
  printf("\n");
  chs(1, c);
}

int main(int argc, char** argv){
  printf("%d\n", sizeof(void));
  def = makeworker(1024);
  chan c = mkchan(int);
  spawn(putstrln("hello from spawn", c));
  go(printf("hello from go\n"));
  printf("waiting on rcv from spawned task\n");
  int i = chr(int, c);
  printf("Finished both spawns, spawn printed %d\n", i);
}
