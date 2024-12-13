#pragma once

#ifndef VERB_HPP
#define VERB_HPP

#include "filter.hpp"
#include "dsp_math.h"

#pragma once

#ifndef VERB_HPP
#define VERB_HPP

#include "filter.hpp"
#include "dsp_math.h"

namespace dspheaders{
  class ChownVerb {
    private:
      unsigned rotate = 0;
      unsigned samplerate = 0;

      // iir comb array
      std::array<Comb, 4> cvec = {
        Comb::init<901> (0.f,0.805 ),
        Comb::init<778> (0.f,0.827f),
        Comb::init<1011>(0.f,0.783f),
        Comb::init<1123>(0.f,0.764f),
      };

      // allpass
      std::array<Comb, 3> avec = {
        Comb::init<125>(0.7, 0.7),
        Comb::init<42>(0.7, 0.7),
        Comb::init<12>(0.7, 0.7),
      };

    public:
      float process(float sample) {
        float sig = 0.f;
        int i = 0;
        for (auto &c: cvec) {
          sig += c.process(sample);
        }

        // rotate++;
        sig/=4;
        for (auto &a: avec) {
          sig = a.process(sig);
        }
        return sig;
      };

      ChownVerb(unsigned samplerate) : samplerate(samplerate) {
        for (auto &c: cvec) c.setDamp(0.3f);
        for (auto &a: avec) a.setDamp(0.0f);
      };
  };


  class SchroederVerb {
    private:
      unsigned rotate = 0;
      unsigned samplerate;
      // float ccoeff[4] = {
      //   .742f, 
      //   .733f, 
      //   .715f, 
      //   .697f, 
      // };
      std::array<Comb, 4> cvec = {
        Comb::init<4799>(.742f, 0.f),
        Comb::init<4999>(.733f, 0.f),
        Comb::init<5399>(.715f, 0.f),
        Comb::init<5801>(.697f, 0.f),
      };


      std::array<Comb, 3> avec = {
        Comb::init<1051>(0.7, 0.7),
        Comb::init<337>(0.7, 0.7),
        Comb::init<113>(0.7, 0.7),
      };

    public:
      float process(float sample) {
        float sig = sample;
        int i = 0;
        for (auto &a: avec) 
          sig = a.process(sig);

        for (auto &c: cvec) 
          sig += c.process(sig);

        return sig/5;
      }

      SchroederVerb(unsigned samplerate) : samplerate(samplerate) {
        for (auto &c: cvec) c.setDamp(0.3f);
        for (auto &a: avec) a.setDamp(0.0f);
      };
  };

  class MoorerVerb {
    private:
      unsigned samplerate;
      // allpass
      Comb initial = Comb::init<6>(0.7, 0.7);
      // iir comb array
      std::array<Comb, 6> cvec = {
        Comb::init<2400>(0.0, .742f),
        Comb::init<2688>(0.f, .733f),
        Comb::init<2928>(0.f, .715f),
        Comb::init<3264>(0.f, .697f),
        Comb::init<3455>(0.f, .683f),
        Comb::init<3744>(0.f, .677f),
      };
      
    public:
      float process(float sample, float length) {
        float sig = sample;
        int i = 0;
        sig = initial.process(sig);
        float out = 0.f;

        unsigned len = length * samplerate;

        for (auto &c: cvec) {
          float amount = dbtorms(
              (
               (
                (
                 (float)cvec[i].getBufferLength() / samplerate
                 ) * 1000
                ) * -60 / len
               ) + 100
              );
          out += c.process(sig);
        }

        return sig;
      }

      MoorerVerb(unsigned samplerate) : samplerate(samplerate) {
        for (auto &c: cvec) c.setDamp(0.1f);
      };
  };
}

#endif
