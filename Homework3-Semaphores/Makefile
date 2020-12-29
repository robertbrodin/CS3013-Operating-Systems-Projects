LIB=-lpthread -lrt
CC=gcc
CCPP=g++

all: addem life

addem: addem.o
	g++ addem.o -o addem -pthread

addem.o: addem.cpp
	g++ -c addem.cpp

life: life.o
	g++ life.o -o life -pthread

life.o: life.cpp
	g++ -c life.cpp

clean:
	rm addem.o life.o