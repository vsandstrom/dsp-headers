#ifndef TABLE_LEN
    #define TABLE_LEN 16
#endif

// TODO: 
// [  ] -> "calcPosition" needs to handle *negative* steps, when the phase input is negative, 
//          like the case is with a bipolar oscillator signal. 

#ifndef PI
  const double PI = 3.14159265358979323846;
#endif


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


struct wavetable {
    double* table;
    double tableLength;
    double position;
    double frequency;
    double samplerate;

    double (*interpolate)(wavetable* self, INTERPOLATION type);
    // double (*calcPosition)(wavetable* self);
    void (*calcPosition)(wavetable* self, double phase);
    // void (*populateTable)(wavetable* self, WAVETYPES type);
};

struct frame {
	float left;
	float right;
};

// instead of #defined math macros
//
//
void populateTable(double* table, int tableLength, WAVESHAPE waveshape);
// doing operations on struct member variables
//
double interpolate(wavetable* table, INTERPOLATION type);
// function with no access to member variables
// double interpolate2(double position, double* table);
//
// no return value, only updates member variables
// 
void calcPosition(wavetable* table, double phase);
void calcPosition(wavetable* table);

// doing operations on member variables, with return value
// double calcPosition2(wavetable* table);
// function with no access to member variables
// double calcPosition3(double samplerate, double frequency, double tableLength);

