#include "wavetable.hpp"

using namespace dspheaders;

Wavetable::Wavetable(
  WAVESHAPE waveshape,
  unsigned tablelength,
  unsigned samplerate,
  float (*interpolate)(float pos, float* table, unsigned length)
): tablelength(tablelength), samplerate((float)samplerate), interpolate(interpolate) {
  table = new float[tablelength+1];
  populatetable(waveshape);
  position = 0.f;
};

Wavetable::Wavetable(
  float* table,
  unsigned tablelength,
  unsigned samplerate,
  float (*interpolate)(float pos, float* table, unsigned length)
): table(table), tablelength(tablelength), samplerate((float)samplerate), interpolate(interpolate) {
  position = 0.f;
};


void Wavetable::movepointer(float phase) {
	float normalizedPhase = (phase + 1) * 0.5;
  position += tablelength / (samplerate / (frequency * normalizedPhase));
  position = wrapf(position, tablelength);
}

void Wavetable::movepointer() {
  position += tablelength / (samplerate / frequency);
  position = wrapf(position, tablelength);
}

float Wavetable::play(){
  float out = interpolate(position, table, tablelength);
  movepointer();
  return out;
}

float Wavetable::play(float phase){
  float out = interpolate(position, table, tablelength);
  movepointer(phase);
  return out;
}


void Wavetable::populatetable(WAVESHAPE waveshape) {
  float inc = 0.f, angle = 0.f, numSamples = (float) tablelength;
  switch (waveshape) {
    case (SINE):
      {
        inc = pi * 2.f  / numSamples;
        for (int i = 0; i < tablelength; ++i) {
          table[i] = sin(angle);
          angle += inc;
        }
        break;
      }

    case (SAW):
      {
        inc = 2.f / numSamples;
        angle = -1.f;
        for (int i = 0; i < tablelength; ++i) {
          table[i] =  angle;
          angle += inc;
        }
        break;
      }


    case (TRIANGLE):
      {
        inc = 2.0 / (numSamples / 2);
        for (int i = 0; i < tablelength; ++i) {
          if ( angle > 1.f || angle < -1.f) {
            inc *= -1.f;
          }
          table[i] = angle;
          angle += inc;
        }
        break;
      }

    case (SQUARE):
      {
        float value = 1.0f;
        for (int i = 0; i < tablelength; ++i) {
          table[i] = value;
          if(i == tablelength/2) {
            value = -1.0f;
          }
        }
        break;
      }
    
    case (ENV):
      // A hanning window style envelope (squared sine)
      {
        inc = pi / numSamples;
        for (int i = 0; i < tablelength; ++i) {
          table[i] = 1.f - cos(angle) * cos(angle); 
          angle += inc;
        }
        break;
      }
    
    case (HANNING):
      // A hanning window style envelope (squared sine)
      {
        inc = pi / numSamples;
        for (int i = 0; i < tablelength; ++i) {
          table[i] = 1.f - cos(angle) * cos(angle); 
          angle += inc;
        }
        break;
      }
  }
}
