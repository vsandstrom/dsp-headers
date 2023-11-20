#pragma once

#include "wavetable.hpp"
#include <vector>

namespace dspheaders {

class VectorOscillator {
  private:
    std::vector<Wavetable> tables;
    float (*interpolate)(float position, float* table, unsigned tablelength) = nullptr;
    unsigned tablelength;
    // crossection is the current sample across tables
    float* crossection;
    float weight;

  public:
    float frequency;

    VectorOscillator();
    // Feed the VectorOscillator with a vector of 'n' WaveTables
    VectorOscillator( 
        std::vector<Wavetable> tables,
        float (*interpolate)(float, float*, unsigned));

    float play(float transfer);
    // Returns the next sample from the vector oscillator. 
    // ----
    //
    // transfer - set the position of the readpointer in the transfer table

    float play(float transfer, float phase);
    // Returns the next sample from the vector oscillator. FM enabled 
    // ----
    //
    // phase - *Phase* or *frequency* modulation input
    // transfer - set the position of the readpointer in the transfer table
};
}
