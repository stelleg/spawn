#include "spawn.h"
#include <stdio.h>

queue q;

void hello(arg a){
  printf("Hello from %d\n", (long ) a);
}

int main(){
  q = alloc_queue(8);
  for(long i=0; i < 16; i++){
    spawn(hello, (arg)i, NULL, &q);
  }
  yield(&q);
}
