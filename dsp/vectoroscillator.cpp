#include "vectoroscillator.hpp"
#include <cstdio>
#include <vector>

using namespace dspheaders;


VectorOscillator::VectorOscillator(
                                  std::vector<WaveTable> tables,
                                  WaveTable transferTable, 
                                  int sampleRate,
                                  INTERPOLATION transferInterpolation
                                  ): 
                                tables(tables),
                                sampleRate(sampleRate),
                                transferTable(transferTable),
                                transferInterpolation(transferInterpolation)
    {
      tableWidth = tables.size() - 1;
      crossection = new float[tables.size()]();
}


float VectorOscillator::play(float transferPhase) {
      float transfer = transferPhase * (float)tableWidth;
      float weight = transferTable.play(transferPhase);

      for (int i = 0; i < tables.size(); ++i) {
        tables[i].setFreq(frequency);
        crossection[i] = tables[i].play();
      }

      for (int i = 0; i < tables.size(); ++i) {
        printf("%f\t ", crossection[i]);
      }
      printf("\n");

      return Interpolation::linear(weight, crossection);
}


float VectorOscillator::play(float phase, float transferPhase) {
      float transfer = transferPhase * (float)tableWidth;
      float weight = transferTable.play(transferPhase);

      for (int i = 0; i < tables.size(); ++i) {
        tables[i].setFreq(frequency);
        crossection[i] = tables[i].play(phase);
      }

      return Interpolation::linear(weight, crossection);
}

