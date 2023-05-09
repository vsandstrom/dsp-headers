#pragma once
#ifndef VECTOROSCILLATOR_HPP
#define VECTOROSCILLATOR_HPP

#include "interpolation.hpp"
#include "wavetable.hpp"
#include <vector>

namespace dspheaders {
    class VectorOscillator {
      private:
        std::vector<WaveTable> tables;
        // WaveTable that is used in the *transfer function*, which curve is supposed to be followed
        // when traversing between tables
        int sampleRate;
        WaveTable transferTable;
        INTERPOLATION transferInterpolation;
        int tableWidth;
        // a vertical crossection containing all current samples from all tables in *tables*
        float* crossection;

      public:
        float frequency;

        // Feed the VectorOscillator with a vector of 'n' WaveTables
        VectorOscillator(std::vector<WaveTable> tables, WaveTable transferTable, int sampleRate, INTERPOLATION transferInterpolation);


        void setFreq(float f) {frequency = f;}
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
    };

} /* namespace dspheaders */

#endif
