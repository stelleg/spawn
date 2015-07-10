#include "spawn.h"

queue q;

void fib(arg a){
  long i = (long)a.r0;
  long* ret = (long*)a.r1;
  if(i <= 2){
    *ret = 1;
    return;
  }
  long j=0, k=0;
  spawn(fib, (arg){(void*)(i-1), &j}, &q);
  fib((arg){(void*)(i-2), &k});
  while(!j) getwork(&q);
  *ret = j + k;
}

int main(){
  q = alloc_queue(1);
  create_workers(0, &q);
  long j, i=35;
  spawn(fib, (arg){(void*)i, &j}, &q);
  while(!j) yield(&q);
  printf("fib %d = %d \n", i, j);
}
