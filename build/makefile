cc = mpicc

AssVec: ../src/main.cpp args.o matrix.o vector.o dictionary.o LossFactory.o gradmanager.o skip-gram-mpi-openmp.o
	$(cc) -o AssVec -std=c++11 -fopenmp ../src/main.cpp args.o matrix.o vector.o dictionary.o LossFactory.o gradmanager.o skip-gram-mpi-openmp.o

#AssVec: ../src/main.cpp args.o matrix.o vector.o
#	${cc} -g -o AssVec -std=c++11 ../src/main.cpp args.o matrix.o vector.o

args.o: ../src/args.cpp
	g++ -std=c++11 -c ../src/args.cpp

matrix.o: ../src/matrix.cpp
	g++ -std=c++11 -c ../src/matrix.cpp

vector.o: ../src/vector.cpp
	g++ -std=c++11 -c ../src/vector.cpp

dictionary.o: ../src/dictionary.cpp
	g++ -std=c++11 -c ../src/dictionary.cpp

#Loss.o: ../src/Loss.cpp
#	g++ -std=c++11 -c ../src/Loss.cpp

LossFactory.o: ../src/LossFactory.cpp
	g++ -std=c++11 -c ../src/LossFactory.cpp

gradmanager.o: ../src/gradmanager.cpp
	g++ -std=c++11 -c ../src/gradmanager.cpp

#skip-gram-mpi-openmp.o: ../src/skip-gram-mpi-openmp.cpp args.o matrix.o vector.o dictionary.o Loss.o LossFactory.o
#	$(cc) -std=c++11 -fopenmp ../src/skip-gram-mpi-openmp.cpp args.o matrix.o vector.o dictionary.o Loss.o LossFactory.o

skip-gram-mpi-openmp.o: ../src/skip-gram-mpi-openmp.cpp args.o matrix.o vector.o dictionary.o gradmanager.o
	$(cc) -std=c++11 -c -fopenmp ../src/skip-gram-mpi-openmp.cpp args.o matrix.o vector.o dictionary.o gradmanager.o