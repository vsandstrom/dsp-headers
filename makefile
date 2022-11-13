compile: test.o sound.o
	g++ test.o sound.o -I/usr/local/include -L/usr/local/lib/ -lportaudio -o test

test.o: 
	g++ -o test.o -c test.cpp 

sound.o:
	g++ -o sound.o -c sound.cpp
