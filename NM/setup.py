from distutils.core import setup
from distutils.extension import Extension

setup(name='nelder_mead',
      ext_modules = [Extension('nelder_mead', sources = ['wrapper.c', 'nm.c'])])
