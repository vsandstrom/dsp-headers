#include "interpolation.hpp"
#include "wavetable.hpp"
#include <vector>

#ifndef VECTOROSCILLATOR_HPP
#define VECTOROSCILLATOR_HPP

namespace dspheaders {

class VectorOscillator {
  private:
    std::vector<WaveTable> tables;
    int sampleRate;
    INTERPOLATION transferInterpolation;
    int tableWidth;
    // a vertical crossection containing all current samples from all tables in *tables*
    float* crossection;
    float weight;

  public:
    float frequency;

    VectorOscillator(int sampleRate);
    // Feed the VectorOscillator with a vector of 'n' WaveTables
    VectorOscillator(std::vector<WaveTable> tables, int sampleRate, INTERPOLATION transferInterpolation);


    float play(float transfer);
    // Returns the next sample from the vector oscillator. 
    // ----
    //
    // transfer - set the position of the readpointer in the transfer table

    float play(float phase, float transfer);
    // Returns the next sample from the vector oscillator. FM enabled 
    // ----
    //
    // phase - *Phase* or *frequency* modulation input
    // transfer - set the position of the readpointer in the transfer table
};
}
#endif
