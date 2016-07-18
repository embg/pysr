#pragma OPENCL EXTENSION cl_khr_fp64: enable
#define MAX_NUM_TREE_NODES 30

//TIME PER EVAL: 1.3e-7 s
#include <cstdio>
#include <cassert>
#include <cmath>
#include <stack>

#define CONST_ID -1
#define ADD_ID -2 //numbering is weird to be consistent with 
#define SUB_ID -3 //how things are on the python side
#define MUL_ID -4
#define DIV_ID -5
#define POW_ID -6
#define LOG_ID -7

#include "stacks.c"

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
float eval(short* individual, double* X, double* constants){
  if (root->type == variable){
    return X[root->id];
  } else if(root->type == constant){
    return constants[root->id];
  } else {
    const float left = eval(root->left, X, constants);
    const float right = eval(root->right, X, constants);\

    float result = 0
    switch (root->op){
    case ADD_ID:
      return left + right;
    case SUB_ID:
      return left - right;
    case MUL_ID:
      return left*right;
    case DIV_ID:
      return left/right;
    case POW_ID:
      return pow(left, right);
    case LOG_ID:
      return log(left)/log(right);
    default:
      assert(false); //we should never get here!
    }
    if (~isinf(result))
      return result;
    else
      return max/min/whatever;
  }
}

void fit(short* deapTrees,
         short* startIndices,
         float* constants,
         float* Xs,
         float* ys,
         uchar* heap){
  int bytesUsed = 0;
  int len;
  ///loop///
  int id = get_global_id(0);
  IntStack intS;
  initializeIntStack(&intS, (int*)malloc(sizeof(int)*MAX_NUM_TREE_NODES));

  NodePtrStack sliceS;
  initializeSliceStack(&intS, (slice*)malloc(sizeof(Node*)*MAX_NUM_TREE_NODES));

  slice* s = (slice*)malloc(sizeof(slice));
  s->start = 0; s->end = len;
  pushSlice(sliceS, fullSlice);
  while(nodeS->height > 0){
    s = popSlice(sliceS);
    int start = s->start; int end = s->end; int len = end-start;
    Node* root = (Node*)malloc(sizeof(Node));
    if (deapTree[0] >= 0){
      root->type = variable;
      root->id = deapTree[start];

      root->left = NULL;
      root->right = NULL;
    } else if (deapTree[start] == CONST_ID){
      static int const_id = 0;
      root->type = constant;
      root->id = const_id;
      const_id++;

      root->left = NULL;
      root->right = NULL;
    } else {
      root->type = primitive;
      root->op = deapTree[start];

      intS.height = 0;
      int rightChildIdx = rightTreeIdx(&deapTree[start], len, &intS);

      slice* left = (slice*)malloc(sizeof(slice));
      slice* right = (slice*)malloc(sizeof(slice));
      
      left->start = start+1; left->end = start+rightChildIdx-1;
      right->start = start+rightChildIdx; right->end = end;
      
      pushSlice(sliceS, left); pushSlice(sliceS, right);
    }
  }


  int main(){
    short deapTree[7] = {-2, -4, 0, -1, -6, 1, -1};

    float X[3] = {4.5,5.4,9.2};
    float y[3] = {1,1,1};
    float residuals[3]; 
  }
