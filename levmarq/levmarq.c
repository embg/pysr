#include "levmarq.h"

#define TOL 1e-30 // smallest value allowed in cholesky_decomp()

// default  values for levmarq()
void levmarq_init(LMstat *lmstat) {
  lmstat->verbose = 0;
  lmstat->max_it = 10;
  lmstat->init_lambda = 0.001;
  lmstat->up_factor = 10;
  lmstat->down_factor = 10;
  lmstat->target_derr = 1e-12;
}


int levmarq(int npar, float *par,
            int ny, double *y, double *dysq,
            float(*func)(float *, int, void *),         
            void(*grad)(double *, float *, int, void *), 
            void *fdata, LMstat *lmstat)
{
  int x,i,j,it,nit,ill,verbose;
  double lambda,up,down,mult,weight,err,newerr,derr,target_derr;
  double h[npar*npar],ch[npar*npar];
#define h(x,y) h[x*npar + y]
  double g[npar],d[npar],delta[npar];
  float newpar[npar];

  verbose = lmstat->verbose;
  nit = lmstat->max_it;
  lambda = lmstat->init_lambda;
  up = lmstat->up_factor;
  down = 1/lmstat->down_factor;
  target_derr = lmstat->target_derr;
  weight = 1;
  derr = newerr = 0; /* to avoid compiler warnings */

  /* calculate the initial error ("chi-squared") */
  err = error_func(par, ny, y, dysq, func, fdata);

  /* main iteration */
  for (it=0; it<nit; it++) {
    printf("%g @ ", err);
    for (int i=0; i<npar; i++){
      printf("%g ", par[i]);
    }printf("\n");
    /* calculate the approximation to the Hessian and the "derivative" d */
    for (i=0; i<npar; i++) {
      d[i] = 0;
      for (j=0; j<=i; j++)
        h(i,j) = 0;
    }
    for (x=0; x<ny; x++) {
      if (dysq) weight = 1/dysq[x]; /* for weighted least-squares */
      grad(g, par, x, fdata);
      for (i=0; i<npar; i++) {
        d[i] += (y[x] - func(par, x, fdata))*g[i]*weight;
        for (j=0; j<=i; j++)
          h(i,j) += g[i]*g[j]*weight;
      }
    }

    /*  make a step "delta."  If the step is rejected, increase
        lambda and try again */
    mult = 1 + lambda;
    ill = 1; /* ill-conditioned? */
    while (ill && (it<nit)) {
      for (i=0; i<npar; i++)
        h(i,i) = h(i,i)*mult;

      ill = cholesky_decomp(npar, ch, h);

      if (!ill) {
        solve_axb_cholesky(npar, ch, delta, d);
        for (i=0; i<npar; i++)
          newpar[i] = par[i] + delta[i];
        newerr = error_func(newpar, ny, y, dysq, func, fdata);
        derr = newerr - err;
        ill = (derr > 0);
      } 
      if (verbose) printf("it = %4d,   lambda = %10g,   err = %10g,   "
                          "derr = %10g\n", it, lambda, err, derr);
      if (ill) {
        mult = (1 + lambda*up)/(1 + lambda);
        lambda *= up;
        it++;
      }
    }
    for (i=0; i<npar; i++)
      par[i] = newpar[i];
    err = newerr;
    lambda *= down;  

    if ((!ill)&&(-derr<target_derr)) break;
  }

  lmstat->final_it = it;
  lmstat->final_err = err;
  lmstat->final_derr = derr;

  return (it==nit);
#undef h
}


/* calculate the error function (chi-squared) */
double error_func(float *par,
                  int ny, double *y, double *dysq,
                  float(*func)(float *, int, void *), void *fdata)
{
  int x;
  double res,e=0;

  for (x=0; x<ny; x++) {
    res = func(par, x, fdata) - y[x];
    if (dysq)  /* weighted least-squares */
      e += res*res/dysq[x];
    else
      e += res*res;
  }
  return e;
}


/* solve the equation Ax=b for a symmetric positive-definite matrix A,
   using the Cholesky decomposition A=LL^T.  The matrix L is passed in "l".
   Elements above the diagonal are ignored.
*/
#define l(x,y) l[x*n + y]
#define a(x,y) a[x*n + y]
void solve_axb_cholesky(int n, double l[n], double x[n], double b[n])
{
  int i,j;
  double sum;

  /* solve L*y = b for y (where x[] is used to store y) */

  for (i=0; i<n; i++) {
    sum = 0;
    for (j=0; j<i; j++)
      sum += l(i,j) * x[j];
    x[i] = (b[i] - sum)/l(i,i);      
  }

  /* solve L^T*x = y for x (where x[] is used to store both y and x) */

  for (i=n-1; i>=0; i--) {
    sum = 0;
    for (j=i+1; j<n; j++)
      sum += l(j,i) * x[j];
    x[i] = (x[i] - sum)/l(i,i);      
  }
}


/* This function takes a symmetric, positive-definite matrix "a" and returns
   its (lower-triangular) Cholesky factor in "l".  Elements above the
   diagonal are neither used nor modified.  The same array may be passed
   as both l and a, in which case the decomposition is performed in place.
*/
int cholesky_decomp(int n, double l[n], double a[n])
{
  int i,j,k;
  double sum;
  l(0,0) = sqrt(a(0,0));
  for (i=0; i<n; i++) {
    for (j=0; j<i; j++) {
      sum = 0;
      for (k=0; k<j; k++)
        sum += l(i,k) * l(j,k);
      l(i,j) = (a(i,j) - sum)/l(j,j);
    }

    sum = 0;
    for (k=0; k<i; k++)
      sum += l(i,k) * l(i,k);
    sum = a(i,i) - sum;
    if (sum<TOL) return 1; /* not positive-definite */
    l(i,i) = sqrt(sum);
  }
  return 0;
}
#undef l
#undef a
