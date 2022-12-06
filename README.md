# Testing out DSP stuff with PortAudio.

The Portaudio library is required.

If downloaded on a Mac using Homebrew, the compile method is:
```bash
$ g++ test.cpp sound.cpp -I/usr/local/include -L/usr/local/lib/ -lportaudio -o test
```
or you could **try** using
```bash
$ make
```
Usage:
```bash
$ ./<program name>  [-c <carrier frequency>] [-m <modulator frequency>]
```

## AM or Envelope
There are two oscillators in test.cpp.
One is sounding and one is modulating the volume of the first. 
If the modulating oscillator is set to an audio-rate frequency, we enter AM.

This can be done by altering the #defines in the source code:
<img width="1680" alt="Skärmavbild 2022-11-13 kl  15 13 12" src="https://user-images.githubusercontent.com/79695983/201526306-e5c02aeb-9240-4b02-8a30-accbd157b6b1.png">
