gcc -c -std=c99 cpu_fitter.c -Ilevmarq
gcc -c -std=c99 levmarq/levmarq.c
gcc -shared  -std=c99 cpu_fitter.o levmarq.o -o lev_test.dll
python test_dll.py
