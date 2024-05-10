CXX = g++
CXXFLAGS = -std=c++2a -Wall -Wextra -Iinclude -Iinclude/pairing_functions -O2 -march=native -mtune=native
GTEST_LIBS = -lgtest -lgtest_main -pthread

all: szudzik

szudzik: src/pairing_functions/szudzik.cpp
	$(CXX) $(CXXFLAGS) -c src/pairing_functions/szudzik.cpp -o szudzik.o

test: tests/pairing_functions/szudzik_test.cpp szudzik
	$(CXX) $(CXXFLAGS) tests/pairing_functions/szudzik_test.cpp szudzik.o $(GTEST_LIBS) -o szudzik_test
	./szudzik_test

clean:
	rm -f szudzik.o szudzik_test
