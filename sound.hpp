
enum WAVETYPES {
    SINE = 0,
    TRIANGLE = 1,
    SAW = 2,
    SQUARE = 3,
    ENV = 4
};

// instead of #defined math macros
const double PI = 3.14159265358979323846;
void populateTable(double* table, int tablelenght, WAVETYPES wavetype);
double interpolate(double position, double* table);
double calcPosition(double samplerate, double frequency, double tablelenght);

struct frame {
	float left;
	float right;
};

struct wavetable {
    double* table;
    double tablelenght;
    double position;
    double frequency;
    double samplerate;
};

