#include <printf.h>
#include <spawn.h>
#include <stdio.h>

void fib(arg a){
  long i = (long)a.r0;
  long* ret = (long*)a.r1;
  if(i <= 2){
    *ret = 1;
    return;
  }
  volatile long j=0, k=0;
  spawn(fib, (arg){(void*)(i-1), (void*)&j});
  fib((arg){(void*)(i-2), (void*)&k});
  while(!j || !k) getwork();
  *ret = j + k;
}

int main(){
  init(1, 1);
  volatile long j=0, i=40;
  spawn(fib, (arg){(void*)i, (void*)&j});
  while(!j) getwork();
  printf("fib %ld = %ld \n", i, j);
}
