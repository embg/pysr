from ctypes import *
import numpy as np
dll = windll.LoadLibrary('lev_test.dll')

x = np.linspace(1, 10)
y = 0 + 1*x + 2*x**2

CONST_ID = -1
ADD_ID = -2 #numbering is weird to be consistent with 
SUB_ID = -3 # how things are on the python side
MUL_ID = -4
DIV_ID = -5
POW_ID = -6
LOG_ID = -7

ind = [CONST_ID, CONST_ID, 0, MUL_ID, ADD_ID, CONST_ID, 0, MUL_ID, 0, MUL_ID, ADD_ID]
c0 = [0,1,2]

ind = (c_short*len(ind))(*ind)
params = (c_float*len(c0))(*c0)
x = (c_float*len(x))(*x)
y = (c_double*len(y))(*y)


# void fit(short* individual, int len,
#          float* params, int numParams,
#          float* X, int numVars,
#          double* y, int numPoints)

print len(c0), len(x), len(y), len(ind)

dll.fit.argtypes = [POINTER(c_short*len(ind)), c_int, POINTER(c_float*len(c0)), c_int, POINTER(c_float*len(x)), c_int, POINTER(c_double*len(y)), c_int]
dll.fit( byref(ind), len(ind), byref(params), len(c0), byref(x), 1, byref(y), len(y))