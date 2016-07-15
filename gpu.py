from deap import gp
import operator, math
import numpy as np
import pyopencl as cl
import pyopencl.tools
import pyopencl.array

maxTreeSize = 25
maxTreeHeight = 7

ctx = cl.create_some_context(interactive=False)
queue = cl.CommandQueue(ctx)

#GPU optimization methods
def convert(tree):
    #Opcodes >=0 are for x_0,x_1...
    #Opcodes <0 are for other kinds of nodes, starting with constant nodes
    #I know, it's a terrible format, but it gets compiled into a real nice recursive
    #struct on the GPU, so don't worry about it.
    def nameToOpcode(node):
        if node.name[0:2] == 'x_:':
            return int(x[2:])
        else:
            return {
                #Don't mess with this unless you understand preamble.c
                #if you do, make sure you keep them negative! Super important.
                'C':-1 
                'add':-2
                'sub':-3
                'mul':-4
                'div':-5
                'pow':-6
                'log':-7
                }[node.name]

    return np.ndarray(list(map(nameToOpcode, tree)), dtype=np.int16)

def 
#WAIT A SEC HOLD ON FAM WE SHOULD COMPILE ON THE GPU AGHGHGHHGHGH WOWOWOWOWOOW
