
compile: 
	# test.o wavetable.o interpolation.o vectoroscillator.o
	clang++ -g -O0 -fno-inline \
		./test.cpp \
		./dsp/wavetable.cpp \
		./dsp/vectoroscillator.cpp \
		./dsp/interpolation.cpp \
		-I/usr/local/include -L/usr/local/lib/ -lportaudio -o ./build/test \
		&& cd build/ \
		&& rm *.o

# test.o: 
# 	g++ -o ./build/test.o -c -g test.cpp 
#
# vectoroscillator.o:
# 	g++ -o ./build/vectoroscillator.o -c -g ./dsp/vectoroscillator.cpp
#
# wavetable.o:
# 	g++ -o ./build/wavetable.o -c -g ./dsp/wavetable.cpp
#
# interpolation.o:
# 	g++ -o ./build/interpolation.o -c -g ./dsp/interpolation.cpp

clean:
	rm -r ./build/*

dir:
	mkdir -p build
