#include <math.h>
#include <assert.h>
#include <stdlib.h>
#define CONST_ID -1
#define ADD_ID -2 //numbering is weird to be consistent with 
#define SUB_ID -3 //how things are on the python side
#define MUL_ID -4
#define DIV_ID -5
#define POW_ID -6
#define LOG_ID -7

#include "stacks.c"
#include "levmarq.h"

#ifndef MAXFLOAT
#define MAXFLOAT      3.40282347e+38F
#endif
float nan_to_num(float num){
  if (isinf(num)){
    if (signbit(num))
      return MAXFLOAT;
    else
      return -MAXFLOAT;
  } else {
    return num;
  }
}

//This function evaluates "root" at (X, CONST_IDs)
float eval(short* individual, int len, float* x, float* params, Stack* S){
  int param_idx = 0;

  for (int i=0; i<len; i++){
    if (individual[i] >= 0){
      push(S, x[i]);
    } else if (individual[i] == CONST_ID){
      push(S, params[param_idx]);
      param_idx++;
    } else {
      float right = pop(S); float left = pop(S);
      switch (individual[i]){
      case ADD_ID:
        push(S, nan_to_num(left + right)); break;
      case SUB_ID:
        push(S, nan_to_num(left - right)); break;
      case MUL_ID:
        push(S, nan_to_num(left*right)); break;
      case DIV_ID:
        push(S, nan_to_num(left/right)); break;
      case POW_ID:
        push(S, nan_to_num(pow(left, right))); break;
      case LOG_ID:
        push(S, nan_to_num(log(left)/log(right))); break;
      default:
        printf("%i", individual[i]);
        assert(0); //we should never get here!
      }
    }
  }
  return pop(S);
}

typedef struct{
  int num_p;
  int num_x;
  float* X;
  short* individual;
  int len;
  Stack* S;
} eval_data;

float func(float* params, int x_idx, void* fdata_void_ptr){
  eval_data* fdata = (eval_data*)fdata_void_ptr;

  float* x = &(fdata->X)[x_idx*fdata->num_x];
  return eval(fdata->individual,
              fdata->len,
              x,
              params,
              fdata->S);
}

void grad(double* G, float* params, int x_idx, void* fdata_void_ptr){
  float f_x = func(params, x_idx, fdata_void_ptr);

  eval_data* fdata = (eval_data*)fdata_void_ptr;
  for (int i=0; i<fdata->num_p; i++){
    float p_i = params[i];
    double h = h_multiplier*(p_i+h_multiplier);

    params[i] = p_i+h;
    float f_x_plus = func(params, x_idx, fdata_void_ptr);
    
    G[i] = (f_x_plus - f_x)/(h);
    params[i] = p_i;
  }
}

void fit(short* individual, int len,
         float* params, int numParams,
         float* X, int numVars,
         double* y, int numPoints){

  eval_data fdata;
  fdata.X = X;
  fdata.individual = individual;
  fdata.len = len;
  fdata.num_p = numParams;
  fdata.num_x = numVars;
  
  Stack S;
  initializeStack(&S, (float*)malloc(sizeof(float)*len));
  fdata.S = &S;

  LMstat lmstat;
  levmarq_init(&lmstat);

  levmarq(numParams, params,
          numPoints, y, NULL,
          &func, &grad, &fdata,
          &lmstat);
}

  #define numParams 1
  #define numPoints 1000
  #define numVars 1
  #define len 3
int main(){
  
  short individual[len] = {-1, 0, -4};

  float gen_params[numParams] = {1};
  double y[numPoints]; float X[numPoints];
  for (int i=0; i<numPoints; i++){
  X[i] = i;
  y[i] = gen_params[0]*X[i];
}
  float params[numParams] = {10};
  fit(individual, len, params, numParams, X, numVars, y, numPoints);
  for(int i=0; i<numParams; i++){
  printf("%g ", params[i]);
}
  printf("\n");
  return 0;
}
