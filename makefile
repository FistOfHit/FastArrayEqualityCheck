CXX = g++
CXXFLAGS = -std=c++2a -Wall -Wextra -Isrc -Iinclude -O2 -march=native -mtune=native
GTEST_LIBS = -lgtest -lgtest_main -pthread

all: szudzik

szudzik: src/szudzik.cpp
	$(CXX) $(CXXFLAGS) -c src/szudzik.cpp -o szudzik.o

test: tests/szudzik_test.cpp szudzik
	$(CXX) $(CXXFLAGS) tests/szudzik_test.cpp szudzik.o $(GTEST_LIBS) -o szudzik_test
	./szudzik_test

clean:
	rm -f szudzik.o szudzik_test
