INCLUDES = /opt/homebrew/include

CXX = clang++
CXXFLAGS = -std=c++14 -Wall -I${INCLUDES} -I./dsp/
BUILDDIR =: $(shell mkdir -p build)

compile: test.o wavetable.o interpolation.o
	${CXX} ${CXXFLAGS} test.o wavetable.o interpolation.o -I/usr/local/include -L/usr/local/lib/ -lportaudio -o test \
		&& rm *.o 

SRCS = ${wildcard *.cpp ./dsp/wavetable.cpp ./dsp/interpolation.cpp ./dsp/vectoroscillator.cpp}
OBJS = $(patsubst %.cpp,build/%.o,${SRCS})

all: ${OBJS} 
	${CXX} ${CXXFLAGS} ${OBJS} -o build/dsp-practice -L/opt/homebrew/lib -lportaudio

build/%.o : %.cpp
	mkdir -p ${dir $@} 
	${CXX} -o $@ $< -c ${CXXFLAGS}

clean:
	rm -r build
