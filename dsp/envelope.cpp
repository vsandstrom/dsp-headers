#include "envelope.hpp"
#include "buffer.hpp"

using namespace dspheaders;


Envelope::Envelope(
    float* breakpoints, unsigned pointlength,
    float* breaktimes, unsigned timeslength,
    unsigned samplerate) 
  : BaseEnvelope(breakpoints, pointlength, breaktimes, timeslength, samplerate){
  float time = 0.f;
  for (unsigned int i = 0; i < timeslength; ++i) {
    // convert seconds to a length of samples
    time += breaktimes[i];
    // breaktimes[i] = size;
    // Find minimal pow2 size of buffer
  } 
  *buffer = Buffer(time, samplerate);
  populateEnvelope();
};


EnvelopeL::EnvelopeL(
    float* breakpoints, unsigned pointlength,
    float* breaktimes, unsigned timeslength,
    unsigned samplerate) 
  : BaseEnvelope(breakpoints, pointlength, breaktimes, timeslength, samplerate){
  float time = 0.f;
  for (unsigned int i = 0; i < timeslength; ++i) {
    time += breaktimes[i];
    // Find minimal pow2 size of buffer
  } 
  *buffer = BufferL(time, samplerate);
  populateEnvelope();
};


EnvelopeC::EnvelopeC(
    float* breakpoints, unsigned pointlength,
    float* breaktimes, unsigned timeslength,
    unsigned samplerate) 
  : BaseEnvelope(breakpoints, pointlength, breaktimes, timeslength, samplerate){
  float time = 0.f;
  for (unsigned int i = 0; i < timeslength; ++i) {
    time += breaktimes[i];
    // Find minimal pow2 size of buffer
  } 
  *buffer = BufferC(time, samplerate);
  populateEnvelope();
};


EnvelopeH::EnvelopeH(
    float* breakpoints, unsigned pointlength, 
    float* breaktimes, unsigned timeslength,
    unsigned samplerate) 
  : BaseEnvelope(breakpoints, pointlength, breaktimes, timeslength, samplerate){
  float time = 0.f;
  for (unsigned int i = 0; i < timeslength; ++i) {
    time += breaktimes[i];
    // Find minimal pow2 size of buffer
  } 
  *buffer = BufferH(time, samplerate);
  populateEnvelope();
};
