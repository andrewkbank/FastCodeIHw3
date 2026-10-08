#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "immintrin.h"

#include "kernel.h"
#include "pack.h"

#define RUNS 2

//timing routine for reading the time stamp counter
static __inline__ unsigned long long rdtsc(void) {
  unsigned hi, lo;
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
  return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}


int main(){

  double *a, *pack_a;
  double *b;
  double *c;

  unsigned long long t0, t1, sum;

  //CHANGE THESE BASED ON YOUR COMPUTED VALUES
  //YOU MAY ASSUME THAT THE VALUE OF MC IS A MULTIPLE OF M.
  //NO EDGE CASES REQUIRED
  
  int mc = 96;  //mc is the number of rows of A
  int k = 256;  //k is the number of columns of A

  //DO NOT CHANGE THESE
  int m  = 6;  //m is the number of rows of the kernel
  int n  = 8;  //n is the number of columns of the kernel

  /*
    Assume the following
        - All matrices are storedin row major order
  */
  posix_memalign((void**) &a, 64, mc * k * sizeof(double));
  posix_memalign((void**) &pack_a, 64, mc * k * sizeof(double));
  posix_memalign((void**) &b, 64, n * k * sizeof(double));
  posix_memalign((void**) &c, 64, mc * n * sizeof(double));
  
    //initialize A
  for (int i = 0; i != k * mc; ++i){
    a[i] = ((double) rand())/ ((double) RAND_MAX);
  }
  //initialize B
  for (int i = 0; i != k * n; ++i){
    b[i] = ((double) rand())/ ((double) RAND_MAX);
  }
  //initialize C
  for (int i = 0; i != mc * n; ++i){
    c[i] = 0.0;
  }
  
  printf("%d\t %d\t %d\t", mc, n, k);

  pack(pack_a, a, mc, m, k);
    
  sum = 0; 
  
  for (int runs = 0; runs != RUNS; ++runs){
    t0 = rdtsc();
    for (int i = 0; i != mc / m; ++i){	
      kernel(k, pack_a + i*m*k, b, c + i*n*m);     
    }
    t1 = rdtsc();
    sum += (t1 - t0);
  }
  
  printf(" %lf\n", (2.0*mc*n*k)/((double)(sum/(1.0*RUNS))));

  free(a);
  free(pack_a);
  free(b);
  free(c);
  
  return 0;
}
