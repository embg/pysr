typedef struct {
  int height;
  float* array;
} Stack;
void initializeStack(Stack* S, float* array){
  S->array = array;
  S->height = 0;
}
void push(Stack* S, float i){
  S->array[S->height] = i;
  S->height++;
}
float pop(Stack* S){
  S->height--;
  return S->array[S->height];
}
void clear(Stack* S){
  S->height = 0;
}
