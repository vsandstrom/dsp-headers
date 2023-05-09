INCLUDES = ${DSPPREFIX}/include

CXX = clang++
CXXFLAGS = -std=c++14 -Wall -I${INCLUDES} -I./dsp/
BUILDDIR =: $(shell mkdir -p build)

SRCS = ${wildcard *.cpp ./dsp/wavetable.cpp ./dsp/interpolation.cpp ./dsp/vectoroscillator.cpp}
OBJS = $(patsubst %.cpp,build/%.o,${SRCS})

all: ${OBJS} 
	${CXX} ${BCXXFLAGS} ${OBJS} -o build/dsp-practice -L${DSPPREFIX}/lib -lportaudio

build/%.o : %.cpp
	mkdir -p ${dir $@} 
	${CXX} -o $@ $< -c ${CXXFLAGS}

clean:
	rm -r build
