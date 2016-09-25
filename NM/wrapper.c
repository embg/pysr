#include "Python.h"
void _minimize(double (*func)(double* x, void* args), double* x0, int N, void* args);

typedef struct arg_struct {
  PyObject* func;
  int N;
} arg_struct;

double func(double* x, void* args){
  //Build a PyList off of x
  int N = ((arg_struct*)args)->N;
  PyObject* x_list = PyList_New(N);
  for (int i=0; i<N; i++){ PyList_SetItem(x_list, i, PyFloat_FromDouble(x[i])); }

  //Pass x_list into the python objective function
  PyObject* arglist = Py_BuildValue("(O)", x_list);
  PyObject* result = PyObject_CallObject(((arg_struct*)args)->func, arglist);
  Py_DECREF(x_list);

  //Convert result to a double and return
  return PyFloat_AsDouble(result);
}

static PyObject* minimize(PyObject* self, PyObject* py_args){
  //Get object pointers to f and x0
  PyObject* py_func, *x0_list;
  if (!PyArg_ParseTuple(py_args, "OO", &py_func, &x0_list)) return NULL;

  //Copy doubles out of x0_list to a regular double* array
  int N = PyList_Size(x0_list);
  double* x0 = (double*)malloc(N*sizeof(double));
  for (int i=0; i<N; i++) x0[i] = PyFloat_AsDouble(PyList_GetItem(x0_list, i));

  //Set up an arg_struct and minimize py_func(x0)
  arg_struct c_args = { py_func, N };
  _minimize(&func, x0, N, (void*)&c_args);

  //Now build a list off of x0 and return it
  PyObject* result_list = PyList_New(N);
  for (int i=0; i<N; i++){ PyList_SetItem(result_list, i, PyFloat_FromDouble(x0[i])); }
  free(x0);
  return result_list;
}

//Initialize the module with the module table and stuff
static PyMethodDef module_methods[] = {
  { "minimize", minimize, METH_VARARGS, "Minimize a function." },
  { NULL, NULL, 0, NULL }
};
PyMODINIT_FUNC
initnelder_mead(void){
  (void) Py_InitModule("nelder_mead", module_methods);
}
