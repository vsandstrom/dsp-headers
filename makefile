postclean: compile
	cd build/ && rm -f *.o

compile: vectest.o wavetable.o interpolation.o vectoroscillator.o
	clang++ -g -O0 -fno-inline \
		build/vectest.o build/wavetable.o build/interpolation.o build/vectoroscillator.o \
		-I/usr/local/include -L/usr/local/lib/ -lportaudio \
		-o build/vectest

vectest.o: 
	clang++ -o ./build/vectest.o -c vectest.cpp 
#
vectoroscillator.o:
	clang++ -o ./build/vectoroscillator.o -c ./dsp/vectoroscillator.cpp

wavetable.o:
	clang++ -o ./build/wavetable.o -c ./dsp/wavetable.cpp
#
interpolation.o:
	clang++ -o ./build/interpolation.o -c ./dsp/interpolation.cpp

clean:
	rm -r build/*

dir:
	mkdir -p build
