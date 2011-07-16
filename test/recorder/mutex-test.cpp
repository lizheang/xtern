// RUN: %llvmgcc %s -g -O0 -c -o %t1.ll -S
// RUN: %projbindir/tern-instr < %t1.ll -o %t2.bc
// RUN: llvm-dis -f %t2.bc

// test the x86 .a libraries
// RUN: llc -o %t2.s %t2.bc
// RUN: g++ -g -o %t2 %t2.s -L %projlibdir -lcommonruntime -lrecruntime -lpthread
// RUN: ./%t2 | sort > %t2.out
// RUN: diff %t2.out %s.out > /dev/null

// stress
// RUN: ./%t2 && ./%t2 && ./%t2  && ./%t2  && ./%t2  && ./%t2  && ./%t2

// test the LLVM .bc modules
// RUN: llvm-ld -o %t3 %t2.bc %projlibdir/commonruntime.bc %projlibdir/recruntime.bc
// RUN: llvm-dis -f %t3.bc
// RUN: llc -o %t3.s %t3.bc
// RUN: g++ -g -o %t3 %t3.s -lpthread
// RUN: ./%t3 | sort > %t3.out
// RUN: diff %t3.out %s.out > /dev/null

// stress
// RUN: ./%t3 && ./%t3 && ./%t3  && ./%t3  && ./%t3  && ./%t3  && ./%t3

#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>

#define N (1000)

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void* thread_func(void *arg) {
  char buf[64];
  int tid = (intptr_t)arg;

  for(unsigned i=0;i<100;++i)
    sched_yield();

  sprintf(buf, "%03d RUNS\n", tid);

  int ret;
  struct timespec   ts;
  struct timeval    tp;

  // test 3 different ways of acquiring a pthread mutex
  switch(tid % 3) {
  case 0:
    pthread_mutex_lock(&m);
    break;
  case 1:
    do{
      ret = pthread_mutex_trylock(&m);
    } while(ret==EBUSY);
    break;
  case 2:
    do {
      gettimeofday(&tp, NULL);
      ts.tv_sec  = tp.tv_sec;
      ts.tv_nsec = tp.tv_usec * 1000 + 100;
      ret = pthread_mutex_timedlock(&m, &ts);
    } while(ret == ETIMEDOUT);
    break;
  }

  write(1, buf, strlen(buf));
  pthread_mutex_unlock(&m);
}

int main() {
  int ret;
  pthread_t th[N];

  for(unsigned i=0; i<N; ++i) {
    ret  = pthread_create(&th[i], NULL, thread_func, (void*)i);
    assert(!ret && "pthread_create() failed!");
  }

  for(unsigned i=0; i<N; ++i)
    pthread_join(th[i], NULL);
}
