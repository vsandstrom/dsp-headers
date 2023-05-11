#include "interpolation.hpp"
#include "dsp.h"

#ifndef PI
  #define PI 
  const float pi = 3.14159265358979323846f;
#endif

#ifndef WAVETABLE_HPP
#define WAVETABLE_HPP


namespace dspheaders {

enum WAVESHAPE {
  SINE,
  TRIANGLE,
  SAW,
  SQUARE,
  ENV
};

class WaveTable {
  private:
    float position;
    //WAVESHAPE waveshape;
    float tableLength;
    float samplerate;
    float* table;
    INTERPOLATION interpolationType;
    
    // Creates one of several simple WaveTable shapes, chosen by the WAVESHAPE argument
    //
    //
    // -- SINE     : Sine waveform
    // -- TRIANGLE : Triangle waveform
    // -- SQUARE   : Square waveform
    // -- SAW      : Saw waveform
    // -- ENV      : Hanning window waveform ( squared sine ), used for Envelopes
    void populateTable(WAVESHAPE waveshape);

  public:
  float frequency;

  void unipolar();


	// Entry-point to let the WaveTable oscillator play. Does interpolation and movement of
    // readpointer under the hood.
	float play();
  
    // Entry-point to let the WaveTable oscillator play. Does interpolation and movement of
    // readpointer under the hood.
    // ----
    //
    // Includes access to phase of the wavetable readpointer, allowing for 
    //
    // Frequency Modulation
	float play(float phase);
	


    // Performs an interpolation between samples to determine what value to return when the 
    // read-pointer tries to read the value two samples.
    // ----
    // -- LINEAR : Weighs two points relative to the readpointer. 
    // ----
    //
    // Uses the WaveTable member **interpolationType** 
    float interpolate();

    // Determines which sample to read next, taking into account the oscillators frequency and the
    // global samplerate, then updates the WaveTable-structs *position* member. 
    // ----
    //
    // This also allows for an optional *phase* input, easily translated to **frequency modulation** (
    // FM ).
    void movePointer(float phase);

    // Determines which sample to read next, taking into account the oscillators frequency and the
    // global samplerate, then updates the WaveTable-structs *position* member. 
    void movePointer();

    // Initializes the WaveTable with a WAVESHAPE, creates a new wavetable with a 
    // **zeroed** float-array of of size **tableLength** and populates it with the chosen **WAVESHAPE**, sets the member variable for which type of 
    // interpolation used, and which samplerate is used on the system.
    // ---- 
    // -- SINE     : Sine waveform
    // -- TRIANGLE : Triangle waveform
    // -- SQUARE   : Square waveform
    // -- SAW      : Saw waveform
    // -- ENV      : Hanning window waveform ( squared sine ), used for Envelopes
    // ----
    //
    // The float-array will be +1 of the tableLength, to mediate the check for out of bounds when
    // interpolation between last and first sample. 
    WaveTable(WAVESHAPE waveshape, int tableLength, int samplerate, INTERPOLATION interpolation);
    
    // Initializes the WaveTable with a user supplied float-array of size 
    // **tableLength**, and sets the member variable
    // for the type of interpolation used
    // ----
    //
    // !! BEWARE !! - When the WaveTable object goes out of scope, it will try to free the table
    // pointer. All float-arrays given should be dynamically allocated.
    WaveTable(float* wavetable, int tableLength, int samplerate, INTERPOLATION interpolation);

    WaveTable(int sampleRate);
};

}
#endif
