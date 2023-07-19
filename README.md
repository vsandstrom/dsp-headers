## DSP HEADERS

A collection of audio DSP header files and examples.

The Portaudio library is required.

Usage:
```bash
$ ./<program name>  [-c <carrier frequency>] [-m <modulator frequency>]
```

The builds below expects the portaudio headers be located in "/usr/local/".
Change the directory to match your configuration*

## Vector Oscillator test:
vectest.cpp runs a demo of a VectorOscillator class, scrolling through all waveform types defined in the
WaveTable class. 
```bash
$ clang++ -o build/fmtest \
    fmtest.cpp dsp/interpolation.cpp dsp/vectoroscillator.cpp dsp/wavetable.cpp \
    -I/usr/local/include -L/usr/local/lib/ -lportaudio \
    && ./build/vectest -v <vector oscillator freq> -e <volume LFO freq>


```

## FM / AM test:
fmtest.cpp runs a demo of an FM / AM synth patch, using the WaveTable class
```bash
$ clang++ -o build/fmtest \
    fmtest.cpp dsp/interpolation.cpp dsp/vectoroscillator.cpp dsp/wavetable.cpp \
    -I/usr/local/include -L/usr/local/lib/ -lportaudio \
    && ./build/fmtest -c <carrier freq> -m <modulator freq> -e <volume LFO freq>

```
