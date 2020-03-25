cc = mpicxx

AssVec: ../src/main.cpp args.o dictionary.o LossFactory.o
	$(cc) -o AssVec -std=c++11 -fopenmp ../src/main.cpp args.o dictionary.o LossFactory.o

args.o: ../src/args.cpp
	g++ -std=c++11 -c ../src/args.cpp

dictionary.o: ../src/dictionary.cpp
	g++ -std=c++11 -c ../src/dictionary.cpp

vector.o: ../src/vector.cpp
	g++ -std=c++11 -c ../src/vector.cpp

gradmanager.o: ../src/gradmanager.cpp vector.o
	g++ -std=c++11 -c ../src/gradmanager.cpp vector.o

matrix.o: ../src/matrix.cpp vector.o
	g++ -std=c++11 -c ../src/matrix.cpp vector.o

Loss.o: ../src/Loss.cpp args.o dictionary.o
	g++ -std=c++11 -c ../src/Loss.cpp args.o dictionary.o

skip-gram-mpi-openmp.o: ../src/skip-gram-mpi-openmp.cpp args.o dictionary.o matrix.o gradmanager.o Loss.o
	$(cc) -std=c++11 -c -fopenmp ../src/skip-gram-mpi-openmp.cpp args.o dictionary.o matrix.o gradmanager.o Loss.o

LossFactory.o: ../src/LossFactory.cpp args.o Loss.o skip-gram-mpi-openmp.o
	g++ -std=c++11 -c ../src/LossFactory.cpp args.o Loss.o skip-gram-mpi-openmp.o