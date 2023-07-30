CC = clang++
CFLAGS = -std=c++14 -g -O1
FINAL = compile_fm compile_delay

INCLUDES = -I/usr/local/include -L/usr/local/lib/ -lportaudio 


postclean: $(FINAL)
	cd build/ && rm -f *.o

compile_fm: fmtest.o wavetable.o interpolation.o vectoroscillator.o
	$(CC) $(CFLAGS) \
		build/fmtest.o build/wavetable.o build/interpolation.o \
		$(INCLUDES) \
		-o build/fmtest

compile_vec: vectest.o wavetable.o interpolation.o vectoroscillator.o
	CC $(CFLAGS) \
		build/vectest.o build/wavetable.o build/interpolation.o build/vectoroscillator.o \
		-I/usr/local/include -L/usr/local/lib/ -lportaudio \
		$(INCLUDES) \
		-o build/vectest
 
compile_delay: delaytest.o interpolation.o buffer.o delay.o
	$(CC) $(CFLAGS) \
		build/delaytest.o build/interpolation.o build/delay.o build/buffer.o \
		$(INCLUDES) \
		-o build/delaytest



delaytest.o: delay.o buffer.o
	$(CC) -o ./build/delaytest.o -c delaytest.cpp
 
delay.o: buffer.o
	$(CC) -o ./build/delay.o -c ./dsp/delay.cpp
 
buffer.o: 
	$(CC) -o ./build/buffer.o -c ./dsp/buffer.cpp

 
vectest.0: wavetable.o vectoroscillator.o
	$(CC) -o ./build/vectest.o -c vectest.cpp
 
fmtest.o: wavetable.o
	$(CC) -o ./build/fmtest.o -c fmtest.cpp
# 
vectoroscillator.o:
	$(CC) -o ./build/vectoroscillator.o -c ./dsp/vectoroscillator.cpp
 
wavetable.o:
	$(CC) -o ./build/wavetable.o -c ./dsp/wavetable.cpp
# 
interpolation.o:
	$(CC) -o ./build/interpolation.o -c ./dsp/interpolation.cpp

clean:
	rm -r build/*

dir:
	mkdir -p build
