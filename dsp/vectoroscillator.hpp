#pragma once
#ifndef VECTOROSCILLATOR_HPP
#define VECTOROSCILLATOR_HPP

#include "interpolation.hpp"
#include "wavetable.hpp"
#include <array>

namespace dspheaders {
    class VectorOscillator {
      public:

        // Feed the VectorOscillator with a vector of 'n' WaveTables
        VectorOscillator(int sampleRate);

        void setFreq(float f);
        float play(float transferPhase);
        // Returns the next sample from the vector oscillator. 
        // ----
        //
        // transferPhase - set the position of the readpointer in the transfer table

        float play(float phase, float transferPhase);
        // Returns the next sample from the vector oscillator. FM enabled 
        // ----
        //
        // phase - *Phase* or *frequency* modulation input
        // transferPhase - set the position of the readpointer in the transfer table

      private:
        std::array<WaveTable,5> tables;
        // WaveTable that is used in the *transfer function*, which curve is supposed to be followed
        // when traversing between tables
        int sampleRate_;
        int tableWidth = 5;
        float frequency;

    };

} /* namespace dspheaders */

#endif
