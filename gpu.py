from deap import gp
import operator, math
import numpy as np
import pyopencl as cl
import pyopencl.tools
import pyopencl.array

maxTreeSize = 25

ctx = cl.create_some_context(interactive=False)
queue = cl.CommandQueue(ctx)

def prefixToPostfix(tree):
    """Convert DEAP's prefix format to an easy-to-parse postfix format."""

    if len(tree) == 1:
        return tree
    
    leftSlice = tree.searchSubtree(1)
    rightSlice = tree.searchSubtree(leftSlice.stop)

    return (prefixToPostfix(gp.PrimitiveTree(tree[leftSlice])) +
            prefixToPostfix(gp.PrimitiveTree(tree[rightSlice])) +
            [tree[0]])

def convert(postfix):
    """Convert a DEAP tree in postfix to a numpy array that can be passed to the GPU

    Opcodes >=0 are for x_0,x_1...
    Opcodes <0 are for other kinds of nodes, starting with constant nodes
    I know, it's a terrible format, but it's easy to pass to and parse on the GPU.
    """ 
    def nameToOpcode(node):
        if node.name[0:2] == 'x_:':
            return int(x[2:])
        else:
            return {
                #Don't mess with this unless you understand preamble.c
                #if you do, make sure you keep them negative! Super important.
                'C':-1,
                'add':-2,
                'sub':-3,
                'mul':-4,
                'div':-5,
                'pow':-6,
                'log':-7
                }[node.name]

    return np.ndarray(list(map(nameToOpcode, postfix)), dtype=np.int16)

