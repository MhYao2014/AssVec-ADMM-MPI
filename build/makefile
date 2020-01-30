cc = mpiCC

#AssVec: ../src/main.cpp args.o matrix.o vector.o
#	$(cc) -o AssVec -std=c++11 -fopenmp ../src/main.cpp args.o matrix.o vector.o
AssVec: ../src/main.cpp args.o matrix.o vector.o
	${cc} -g -o AssVec -std=c++11 ../src/main.cpp args.o matrix.o vector.o


args.o: ../src/args.cpp
	g++ -std=c++11 -c ../src/args.cpp

matrix.o: ../src/matrix.cpp
	g++ -std=c++11 -c ../src/matrix.cpp

vector.o: ../src/vector.cpp
	g++ -std=c++11 -c ../src/vector.cpp

