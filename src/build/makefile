cc = mpiCC

AssVec: ../main.cpp args.o matrix.o vector.o
	$(cc) -o AssVec -std=c++11 -fopenmp ../main.cpp args.o

args.o: ../args.cpp
	g++ -std=c++11 -c ../args.cpp

matrix.o: ../matrix.cpp
	g++ -std=c++11 -c ../matrix.cpp

vector.o: ../vector.cpp 
	g++ -std=c++11 -c ../vector.cpp
