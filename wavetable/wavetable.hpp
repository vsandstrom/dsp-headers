#ifndef PI
const float PI = 3.14159265358979323846;
#endif

struct frame {
  float left;
  float right;
};

enum WAVESHAPE {
  SINE = 0,
  TRIANGLE = 1,
  SAW = 2,
  SQUARE = 3,
  ENV = 4
};

enum INTERPOLATION {
  LINEAR = 0,
  COSINE,
  CUBIC,
  HERMITE
};

class WaveTable {
  public:
    float* table;
    float tableLength;
    float position;
    float frequency;
    float samplerate;
    INTERPOLATION interpolationType;

    // Creates one of several simple WaveTable shapes, chosen by the WAVESHAPE enum argument
    //
    //
    // -- SINE     : Sine waveform
    // -- TRIANGLE : Triangle waveform
    // -- SQUARE   : Square waveform
    // -- SAW      : Saw waveform
    // -- ENV      : Hanning window waveform ( squared sine ), used for Envelopes
    void populateTable(WAVESHAPE waveshape);

    // Performs an interpolation between samples to determine what value to return when the 
    // read-pointer tries to read the value two samples.
    // ----
    //
    // Uses the WaveTable member **interpolationType** 
    float interpolate();

    // Determines which sample to read next, taking into account the oscillators frequency and the
    // global samplerate, then updates the WaveTable-structs *position* member. 
    // ----
    //
    // This also allows for an optional *phase* input, easily translated to frequency modulation, 
    // or *phase modulation*.
    void calcPosition(float phase);

    // Determines which sample to read next, taking into account the oscillators frequency and the
    // global samplerate, then updates the WaveTable-structs *position* member. 
    void calcPosition();

    // Initializes the WaveTable with a **zeroed** float-array of of size **tableLength**, 
    // and sets the member variable
    // for the interpolation
    // ----
    //
    // The float-array will be +1 of the tableLength, to mediate the check for out of bounds when
    // interpolation between last and first sample. 
    WaveTable(WAVESHAPE waveshape, int tableLength, INTERPOLATION interpolation, int samplerate);

    // Initializes the WaveTable with a user supplied float-array of size 
    // **tableLength**, and sets the member variable
    // for the type of interpolation used
    // ----
    //
    // !! BEWARE !! - When the WaveTable object goes out of scope, it will try to free the table
    // pointer. All float-arrays used should be dynamically allocated.
    WaveTable(float* wavetable, int tableLength, INTERPOLATION interpolation, int samplerate);

    // Frees the float-array
    ~WaveTable();
};
