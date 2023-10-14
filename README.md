## DSP HEADERS

A collection of audio DSP header files and examples.

The Portaudio library is required.

The builds below expects the portaudio headers be located in <p>"/usr/local/"</p>
(Building portaudio from source currently fails on macOS, but works through Homebrew.)

Run:
```bash
# run make to compile dependencies and examples
$ make
# will compile a demo and other tests in a folder called build/

```
## DEMO:
demotest showcases some of the sounding objects, such as wavetable FM, envelopes and delay.
```bash
$ ./build/demotest
```

<!-- ## Vector Oscillator test: -->
<!-- vectest.cpp runs a demo of a VectorOscillator class, scrolling through all waveform types defined in the -->
<!-- WaveTable class.  -->
<!-- ```bash -->
<!-- $ ./build/vectest -v <vector oscillator freq> -e <volume LFO freq> -->
<!-- ``` -->
<!---->
<!-- ## FM / AM test: -->
<!-- fmtest.cpp runs a demo of an FM / AM synth patch, using the WaveTable class -->
<!-- ```bash -->
<!-- $ ./build/fmtest -c <carrier freq> -m <modulator freq> -e <volume LFO freq> -->
<!-- ``` -->
<!---->
<!-- ## Delay test: -->
<!-- Runs a 30 sec demo of a delay patch using a buffer and cubic interpolation. (beware of feedback) -->
<!-- ```bash -->
<!-- $ ./build/delaytest -t <delaytime> -f <feedback> -->
<!-- ``` -->
<!---->
<!-- ## Envelope test: -->
<!-- A demo of a volume envelope, triggering twice on a timeline. -->
<!-- ```bash -->
<!-- $ ./build/envtest -->
<!-- ``` -->
