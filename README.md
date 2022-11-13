# Testing out DSP stuff with PortAudio.

The Portaudio library is required.

If downloaded on mac with Homebrew, the compile method is:
```bash
$ g++ test.cpp sound.cpp -I/usr/local/include -L/usr/local/lib/  -lportaudio -o test
```
or you could **try** using
```bash
$ make
```


