#include "spawn.h"
#include <stdio.h>

queue q;

void hello(arg a){
  printf("Hello from %d\n", (long ) a.r0);
}

int main(){
  q = alloc_queue(8);
  for(long i=0; i < 16; i++){
    spawn(hello, (arg){(void*)i}, &q);
  }
  yield(&q);
}
