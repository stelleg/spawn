#include "spawn.h"

queue q;

void fib(arg a){
  long i = (long)a.r0;
  long* ret = (long*)a.r1;
  if(i <= 2){
    *ret = 1;
    return;
  }
  volatile long j=0, k=0;
  spawn(fib, (arg){(void*)(i-1), &j}, &q);
  fib((arg){(void*)(i-2), &k});
  while(!j || !k) getwork(&q);
  *ret = j + k;
}

int main(){
  q = alloc_queue(1);
  create_workers(1, &q);
  volatile long j=0, i=35;
  spawn(fib, (arg){(void*)i, &j}, &q);
  while(!j) yield(&q);
  printf("fib %ld = %ld \n", i, j);
}
