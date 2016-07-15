#include <stdio.h>
#include <math.h>

#ifndef __LEVMARQ__
#define __LEVMARQ__

#define X(r, c) (X[(r)*numVars + (c)])
#define h_multiplier 9e-8
typedef struct {
  double* X;
  double (*f)(double* params, double* X);
  int num_x;
  int num_p;
} fdata_struct;

typedef struct {
	int verbose;
	int max_it;
	double init_lambda;
	double up_factor;
	double down_factor;
	double target_derr;
	int final_it;
	double final_err;
	double final_derr;
} LMstat;

double func(double* params, int x_idx, void* fdata);
void grad(double* G, double* params, int x_idx, void* fdata);

void levmarq_init(LMstat *lmstat); //initializes lmstat to default values

double error_func(double *par,
                  int ny, double *y, double *dysq,
                  double(*func)(double *, int, void *), void *fdata);

int ldl_cholesky(int n, double *L, double *A);

// npar    number of parameters
// par     array of parameters to be varied
// ny      number of measurements to be fit
// y       array of measurements
// dysq    array of error in measurements, squared
// (set dysq=NULL for unweighted least-squares)
// func    function to be fit
// grad    gradient of "func" with respect to the input parameters
// fdata   pointer to any additional data required by the function
// lmstat  pointer to the "status" structure, where minimization parameters
// are set and the final status is returned.
int levmarq(int npar, double *par,
            int ny, double *y, double *dysq,
            double(*func)(double *, int, void *),         
            void(*grad)(double *, double *, int, void *), 
            void *fdata, LMstat *lmstat);

void solve_axb_cholesky(int n, double l[n], double x[n], double b[n]);

int cholesky_decomp(int n, double l[n], double a[n]);

#endif
