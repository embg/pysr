#include "levmarq.h"
#include <fenv.h>
double f(double* params, double* x){
  return params[2]*(params[0]*x[0] +
                    params[1]*x[1] +
                    params[0]*params[1]*x[0]*x[1] +
                    params[0]*sin(params[1]*x[0]*x[1]) +
                    params[3]*cos(params[2]*x[0]));
}

#define numVars 4
#define numPoints 1000
#define numParams 4

int main(){
  feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);
  double X[numVars*numPoints]; double y[numPoints];
  double gen_params[numParams] = {2000, 1000, 1, 2};
  for (int i=0; i<numPoints; i++){
    X(i,0) = i*sin(i); X(i,1) = i*sin(2*i);
    y[i] = f(gen_params, &X(i,0));
  }

  LMstat lmstat;
  levmarq_init(&lmstat);

  fdata_struct fdata;
  fdata.X = X;
  fdata.f = &f;
  fdata.num_p = numParams;
  fdata.num_x = numVars;

  double params[numParams] = {-2001, 10001, -10, -2};
  levmarq(numParams, params,
          numPoints, y, NULL,
          &func, &grad, &fdata,
          &lmstat);

  for(int i=0; i<numParams; i++){
    printf("%g ", params[i]);
  }
  printf("\n");
  return 0;
}

