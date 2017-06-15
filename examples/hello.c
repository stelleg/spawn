#include <spawn.h>
#include <stdio.h>
#include <pthread.h>

void hello(arg a){
  printf("Hello from iteration %ld on thread %ld\n", (long ) a.r0, pthread_self());
  *(long*)a.r1 += 1;
}

int main(){
  init(4, 3);
  long num_iters = 16;
  volatile long counter = 0; 
  for(long i=0; i < num_iters; i++){
    spawn(hello, (arg){(void*)i, (void*)&counter});
  }
  while(counter < num_iters) getwork();
  printf("All hellos have been said\n");
}
