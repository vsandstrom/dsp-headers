
// instead of #defined math macros
const double PI = 3.14159265358979323846;
void populateTable(double*, int, int);
double interpolate(double, double*);
enum WAVETYPES {
    SINE = 0,
    TRIANGLE,
    SAW,
    SQUARE,
    ENV
};
