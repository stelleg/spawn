#include "spawn.h"

queue q;

void fib(arg a){
  long i = (long)a.r0;
  long* ret = (long*)a.r1;
  if(i <= 2){
    *ret = 1;
    return;
  }
  long j, k;
  spawn(fib, (arg){(void*)i-1, &j}, &q);
  fib((arg){(void*)i-2, &k});
  *ret = j + k;
}

int main(){
  q = alloc_queue(1);
  create_workers(1, &q);
  spawn(fib, (arg){(void*)30}, &q);
  printf("fib %d = %d \n", 30, 9);
}
