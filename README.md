[![CC BY-NC-SA 4.0][cc-by-nc-sa-shield]][cc-by-nc-sa]

## DSP HEADERS

A collection of audio DSP header files and examples.

Run:
```bash
# download the portaudio submodule
$ git submodule update --init --recursive
# compile the portaudio library
$ cd portaudio && ./configure && make && cd ..
# build the synth and audio effects tests
$ make
# will compile a demo and other tests in a folder called build/

```
## DEMO:
demotest showcases some of the sounding objects, such as wavetable FM, envelopes and delay.
```bash
$ ./build/demotest
```


This work is licensed under a
[Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License][cc-by-nc-sa].

[![CC BY-NC-SA 4.0][cc-by-nc-sa-image]][cc-by-nc-sa]

[cc-by-nc-sa]: http://creativecommons.org/licenses/by-nc-sa/4.0/
[cc-by-nc-sa-image]: https://licensebuttons.net/l/by-nc-sa/4.0/88x31.png
[cc-by-nc-sa-shield]: https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg
