postclean: compile
	cd build/ && rm -f *.o

compile: delaytest.o wavetable.o interpolation.o vectoroscillator.o buffer.o delay.o
	clang++ -g -O0 -fno-inline \
		build/delaytest.o build/wavetable.o build/interpolation.o build/vectoroscillator.o build/delay.o build/buffer.o \
		-I/usr/local/include -L/usr/local/lib/ -lportaudio -ggdb \
		-o build/delaytest

delay.o: buffer.o
	clang++ -o ./build/delay.o -c ./dsp/delay.cpp

buffer.o: 
	clang++ -o ./build/buffer.o -c ./dsp/buffer.cpp

delaytest.o: 
	clang++ -o ./build/delaytest.o -c delaytest.cpp 
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
