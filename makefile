all:
	g++ -I ./lib -c location.cpp
	g++ -o location location.o
	rm -f *.o
	./location
