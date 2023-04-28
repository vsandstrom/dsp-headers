compile: test.o wavetable.o
	g++ test.o wavetable.o -I/usr/local/include -L/usr/local/lib/ -lportaudio -o test \
		&& rm *.o 

test.o: 
	g++ -o test.o -c test.cpp 

wavetable.o:
	g++ -o wavetable.o -c wavetable.cpp -DP
