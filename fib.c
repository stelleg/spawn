#include "spawn.h"

queue q;

typedef struct {
  int i;
  int r;
} fibarg;

void fib(arg a){
  fibarg* f = (fibarg*)a;
  bool ret = 0; 
  //if(f->i > 10) printf("%d\n",f->i);
  if(f->i <= 2){
    f->r = 1;
    return;
  }
  fibarg k = (fibarg) {f->i-1, 0};
  fibarg j = (fibarg) {f->i-2, 0};
  spawn(fib, (arg)&j, &ret, &q);
  fib((arg)&k);
  waiton(&ret, &q);
  f->r = j.r + k.r;
}

int main(){
  q = alloc_queue(1);
  create_workers(1, &q);
  bool ret;
  fibarg i = (fibarg){35, 0};
  spawn(fib, (arg)&i, &ret, &q);
  waiton(&ret, &q);
  printf("fib %d = %d \n", i.i, i.r);
}
