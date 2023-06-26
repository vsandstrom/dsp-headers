postclean: compile
	cd build/ && rm -f *.o

compile: vectest.o wavetable.o interpolation.o vectoroscillator.o delay.o buffer.o
	clang++ -g -O0 -fno-inline \
		build/delaytest.o build/wavetable.o build/interpolation.o build/vectoroscillator.o build/buffer.o build/delay.o \
		-I/usr/local/include -L/usr/local/lib/ -lportaudio \
		-o build/delaytest

vectest.o: 
	clang++ -o ./build/delaytest.o -c delaytest.cpp 
#
vectoroscillator.o:
	clang++ -o ./build/vectoroscillator.o -c ./dsp/vectoroscillator.cpp

delay.o: buffer.o
	clang++ -o ./build/delay.o -c ./dsp/delay.cpp

buffer.o:
	clang++ -o ./build/buffer.o -c ./dsp/buffer.cpp

wavetable.o:
	clang++ -o ./build/wavetable.o -c ./dsp/wavetable.cpp
#
interpolation.o:
	clang++ -o ./build/interpolation.o -c ./dsp/interpolation.cpp

clean:
	rm -r build/*

dir:
	mkdir -p build
