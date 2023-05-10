#include "vectoroscillator.hpp"
#include <cstdio>
#include <vector>
#include <cmath>

using namespace dspheaders;


VectorOscillator::VectorOscillator(
                                  int sampleRate
                                  ): 
                                sampleRate_(sampleRate)
    {

    std::vector<WAVESHAPE> waves {SINE, TRIANGLE, SQUARE, SAW};

    for( auto &table : tables) {
        table = WaveTable(sampleRate_);
    }
    
    for (int i = 0; i < tableWidth; ++i ) {
        tables[i].setFreq(440.f);
        tables[i].setWave(waves[i%4]);
    }
}

void VectorOscillator::setFreq(float f) {
    for (auto &w : tables)
        w.setFreq(f);
    }

float VectorOscillator::play(float transferPhase) {
    
        float ph = fabs(transferPhase); 
        float out = 0.f;
        float inc = 1.f/tableWidth;
        for(int i = 0; i < tableWidth; ++i) {
           out += fmax(1 - fabs(ph - float(i)*inc), 0) * tables[i].play(); 
        }
        return out;
}


float VectorOscillator::play(float phase, float transferPhase) {

        return 0.f;
}

