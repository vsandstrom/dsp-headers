#pragma once

#include "interpolation.hpp"
#include "wavetable.hpp"
#include <vector>
#include "dsp.h"

namespace dspheaders {

class VectorOscillator {
  private:
    std::vector<WaveTable> tables;
    INTERPOLATION transferInterpolation;
    uint32_t tableWidth;
    // crossection is the current sample across tables
    float* crossection;
    float weight;

  public:
    float frequency;

    VectorOscillator();
    // Feed the VectorOscillator with a vector of 'n' WaveTables
    VectorOscillator(
      std::vector<WaveTable> tables, INTERPOLATION transferInterpolation
    );


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
