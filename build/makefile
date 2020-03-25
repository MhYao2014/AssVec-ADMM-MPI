cc = mpicc

AssVec: ../src/main.cpp args.o dictionary.o LossFactory.o
	$(cc) -o AssVec -std=c++11 -fopenmp ../src/main.cpp args.o dictionary.o LossFactory.o

args.o: ../src/args.cpp
	g++ -std=c++11 -o ../src/args.cpp

dictionary.o: ../src/dictionary.cpp
	g++ -std=c++11 -o ../src/dictionary.cpp

vector.o: ../src/vector.cpp
	g++ -std=c++11 -o ../src/vector.cpp

gradmanager.o: ../src/gradmanager.cpp vector.o
	g++ -std=c++11 -o ../src/gradmanager.cpp vector.o

matrix.o: ../src/matrix.cpp vector.o
	g++ -std=c++11 -o ../src/matrix.cpp vector.o

Loss.o: ../src/Loss.cpp args.o dictionary.o
	g++ -std=c++11 -o ../src/Loss.cpp args.o dictionary.o

skip-gram-mpi-openmp.o: ../src/skip-gram-mpi-openmp.cpp args.o dictionary.o matrix.o gradmanager.o Loss.o
	$(cc) -std=c++11 -o -fopenmp ../src/skip-gram-mpi-openmp.cpp args.o dictionary.o matrix.o gradmanager.o Loss.o

LossFactory.o: ../src/LossFactory.cpp
	g++ -std=c++11 -o ../src/LossFactory.cpp