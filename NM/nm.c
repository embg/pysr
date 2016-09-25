#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define sim(x,y) sim[N*(x) + (y)]

//Numpy functions, don't waste your time here
double* zeros(int x, int y){
  return (double*)calloc(x*y,sizeof(double));
}
void copy(double* from, double* to, int len)
{ for(int i=0; i<len; i++) to[i] = from[i]; }
void take(double* arr, int* ind, int numRows, int numCols){
  double* temp = (double*)malloc(numRows*numCols*sizeof(double));
  for(int i=0; i<numRows; i++) copy(&arr[ind[i]*numCols], &temp[i*numCols], numCols);
  copy(temp, arr, numRows*numCols);
  free(temp);
}
void argsort(int* ind, double* A, int n){
  for(int i=0; i<n; i++) ind[i] = i;
  for(int i=0; i<n; i++){
    int x = ind[i];
    int j = i-1;
    while (j >= 0 && A[ind[j]] > A[x]){
      ind[j+1] = ind[j];
      j--;
    }
    ind[j+1] = x;
  }
}

//Beginning of fitter -- corresponds 1 to 1 with the scipy implementation
void _minimize(double (*func)(double* x, void* args), double* x0, int N, void* args)
{
  double
  xatol=1e-4,
  fatol=1e-4,
  rho=1,
  chi=2,
  psi=0.5,
  sigma=0.5,
  nonzdelt=0.05,
  zdelt=0.00025;

  int maxiter = N*200;

  double* sim = zeros(N+1,N);
  double* fsim = zeros(N+1,1);
  copy(x0, &sim(0,0), N);
  fsim[0] = func(x0, args);

  double* y = zeros(N,1);
  for(int k=0; k<N; k++){
    copy(x0, y, N);
    if (y[k] != 0){
      y[k] = (1+nonzdelt)*y[k];
    } else {
      y[k] = zdelt;
    }
    copy(y, &sim(k+1,0), N);
    fsim[k+1] = func(y,args);
  }

  int* ind = (int*)malloc((N+1)*sizeof(int));
  argsort(ind, fsim, N+1);
  take(fsim, ind, N+1, 1);
  take(sim, ind, N+1, N);

  int iterations = 1;

  while (iterations < maxiter){
    double s_max = 0; double f_max = 0;
    for (int i=1; i<N+1; i++)
      if (fabs(fsim[i] - fsim[0]) > f_max) f_max = fabs(fsim[i]-fsim[0]);
    for (int i=1; i<N+1; i++)
      for (int j=0; j<N; j++)
        if (fabs(sim(i,j) - sim(0,j)) > s_max) s_max = fabs(sim(i,j)-sim(0,j));
    if (s_max <= xatol && f_max <= fatol) break;

    double* xbar = zeros(N,1);
    for (int i=0; i<N; i++){
      for (int j=0; j<N; j++)
        xbar[j] += sim(i,j)/N;
    }

    double* xr = zeros(N,1);
    for (int i=0; i<N; i++) xr[i] = (1+rho)*xbar[i] - rho*sim(N,i);
    double fxr = func(xr, args);

    int doshrink = 0;

    if (fxr < fsim[0]){
      double* xe = zeros(N,1);
      for (int i=0; i<N; i++) xe[i] = (1+rho*chi)*xbar[i] - (rho*chi)*sim(N,i);
      double fxe = func(xe, args);

      if (fxe < fxr){
        copy(xe, &sim(N,0), N);
        fsim[N] = fxe;
      } else {
        copy(xr, &sim(N,0), N);
        fsim[N] = fxr;
      }
      free(xe);
    } else {
      if (fxr < fsim[(N+1)-2]){
        copy(xr, &sim(N,0), N);
        fsim[N] = fxr;
      } else {
        if (fxr < fsim[N]){
          double* xc = zeros(N,1);
          for (int i=0; i<N; i++) xc[i] = (1+psi*rho)*xbar[i] - (psi*rho)*sim(N,i);
          double fxc = func(xc, args);

          if (fxc <= fxr){
            copy(xc, &sim(N,0), N);
            fsim[N] = fxc;
          } else {
            doshrink = 1;
          }
          free(xc);
        } else {
          double* xcc = zeros(N,1);
          for (int i=0; i<N; i++) xcc[i] = (1-psi)*xbar[i] + psi*sim(N,i);
          double fxcc = func(xcc, args);

          if (fxcc < fsim[N]){
            copy(xcc, &sim(N,0), N);
            fsim[N] = fxcc;
          } else {
            doshrink = 1;
          }
          free(xcc);
        }

        if (doshrink) {
          for (int i=1; i<N+1; i++) {
            for (int j=0; j<N; j++) sim(i,j) = sim(0,j) + sigma*(sim(i,j) - sim(0,j));
            fsim[i] = func(&sim(i,0), args);
          }
        }
      }
    }

    argsort(ind, fsim, N+1);
    take(sim, ind, N+1, N);
    take(fsim, ind, N+1, 1);

    iterations++;
    free(xbar); free(xr);
  }
  copy(&sim(0,0), x0, N);
  free(ind); free(sim); free(y); free(fsim);
//  printf("Iterations: %i\n", iterations);
}
