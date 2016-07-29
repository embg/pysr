#include "nm.h"

//Numpy functions
void copy(double* from, double* to, int len)
{ for(int i=0; i<len; i++) to[i] = from[i]; }
void take(double* arr, int* ind, int numRows, int numCols){
  double* temp = (double*)malloc(numRows*numCols*sizeof(double));
  for(int i=0; i<numRows; i++) copy(&arr[ind[i]*numCols], &temp[i*numCols], numCols);
  copy(temp, arr, numRows*numCols);
}
void argsort(    
double minimize(double (*func)(double* x), double* x0, int N)
{
  //Declare variables
  double rho=1, chi=2, psi=0.5, sigma=0.5, nonzdelt=0.05, zdelt=0.00025;
  double* sim = (double*)malloc((N+1)*N*sizeof(double));
#define sim(x,y) sim[(N+1)*x + y]

  //Construct initial simplex
  for(int k=0; k<N; k++) sim(0, k) = x0[k];
  double* y = (double*)malloc(N*sizeof(double));
  for(int k=0; k<N; k++){
    copy(x0, y, N);
    if (y[k] != 0)
      y[k] = (1 + nonzdelt)*y[k];
    else
      y[k] = zdelt;
    copy(y, &sim(k+1,0), N);
  }

  //Set defaults for maxiter and maxfun
  int maxiter = N*200; int maxfun = N*200;

  //Begin the main loop
  double* fsim = (double*)malloc((N+1)*sizeof(double));
  for(int k=0; k<N+1; k++) fsim[k] = func(&sim(k,0));
  
