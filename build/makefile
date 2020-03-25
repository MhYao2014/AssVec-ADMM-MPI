cc = mpiCC

AssVec: ../src/main.cpp args.o matrix.o vector.o dictionary.o LossFactory.o SkipGramMpiOpenmp.o
	$(cc) -o AssVec -std=c++11 -fopenmp ../src/main.cpp args.o matrix.o vector.o dictionary.o LossFactory.o SkipGramMpiOpenmp.o

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

#SkipGramMpiOpenmp.o: ../src/skip-gram-mpi-openmp.cpp args.o matrix.o vector.o dictionary.o Loss.o LossFactory.o
#	$(cc) -std=c++11 -fopenmp ../src/skip-gram-mpi-openmp.cpp args.o matrix.o vector.o dictionary.o Loss.o LossFactory.o

SkipGramMpiOpenmp.o: ../src/skip-gram-mpi-openmp.cpp args.o matrix.o vector.o dictionary.o LossFactory.o
	$(cc) -std=c++11 -fopenmp ../src/skip-gram-mpi-openmp.cpp args.o matrix.o vector.o dictionary.o LossFactory.o