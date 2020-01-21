cc = mpiCC

AssVec: main.cpp args.o
	$(cc) -o AssVec -std=c++11 -fopenmp main.cpp args.o

args.o: args.cpp
	g++ -std=c++11 -c args.cpp
