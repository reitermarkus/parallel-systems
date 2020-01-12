#include <stdio.h>
#include <math.h>

#ifdef _OPENMP
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <omp.h>
#pragma GCC diagnostic pop
#endif

#include "type.h"

void print_results(char class, int n1, int n2, int n3, int niter,
    double t, double mops, char *optype, logical verified, char *npbversion)
{
  printf( "\n\n Benchmark Completed.\n");
  printf( " Class           =             %12c\n", class );

  // If this is not a grid-based problem (EP, FT, CG), then
  // we only print n1, which contains some measure of the
  // problem size. In that case, n2 and n3 are both zero.
  // Otherwise, we print the grid size n1xn2xn3

  if ( ( n2 == 0 ) && ( n3 == 0 ) ) {
    printf( " Size            =             %12d\n", n1 );
  } else {
    printf( " Size            =           %4dx%4dx%4d\n", n1, n2, n3 );
  }

  printf( " Iterations      =             %12d\n", niter );
  printf( " Time in seconds =             %12.2lf\n", t );

  printf( " Mop/s total     =          %15.2lf\n", mops );

  #ifdef _OPENMP
  #pragma omp parallel
  {
    #pragma omp single
    printf( " Mop/s/thread    =          %15.2lf\n", mops/(double)omp_get_num_threads() );
  }
  #endif

  printf( " Operation type  = %24s\n", optype );
  printf( " Verification    =             %12s\n", verified ? "SUCCESSFUL" : "UNSUCCESSFUL");
  printf( " Version         =             %12s\n", npbversion );
}

