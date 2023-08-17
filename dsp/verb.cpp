#include "dsp.h"
#include "interpolation.hpp"
#include "verb.hpp"
#include "delay.hpp"
#include <__config>
#include <cstdio>
#include <x86intrin.h>

using namespace dspheaders;

// Verb::Verb(
//     unsigned samplerate,
//     float feedback,
//     float (*interpolate)(float, float*, unsigned)
//   ): Delay(Delay(samplerate, 8.f, interpolate)), fb(feedback), samplerate(samplerate) {
// };
//
// float Verb::play(float sample) {
//   float out = 0.f;
//   out = read(i[3]/samplerate) * k[3];
//   out += read(i[2]/samplerate) * k[2];
//   out += read(i[1]/samplerate) * k[1];
//   out += read(i[0]/samplerate) * k[0];
//   write(sample + (out * fb));
//   writeptr++;
//   return out;
// }
//
// void Verb::feedback(double feedback){
//   fb = feedback;
// }

// float Verb::play(float sample) {
//   buffer.writesample(sample, writeptr);
//
//   printf("before indicies");
//   // get new indicies
//   __m128i idx = _mm_load_si128((__m128i*)i);
//   __m128i wptrs = _mm_set1_epi32(writeptr);
//   __m128i newIdx = _mm_sub_epi32(idx, wptrs);
//   _mm_store_si128((__m128i*)i, newIdx);
//   writeptr++;
//
//   printf("before reading from buffer");
//   // read those indicies 
//   __m128 s = _mm_set_ps(
//     buffer.readsample(i[0]),
//     buffer.readsample(i[1]),
//     buffer.readsample(i[2]),
//     buffer.readsample(i[3])
//   );
//
//   printf("before loading previous values");
//   // use the previous values and store the
//   __m128 prev = _mm_load_ps(v);
//
//   printf("before calculating next values");
//   __m128 fb_v = _mm_mul_ps(prev, fb);
//   __m128 s2 = _mm_add_ps(fb_v, s);
//   __m128 factor = _mm_load_ps(k);
//   __m128 result = _mm_mul_ps(s2, factor);
//   _mm_store_ps(v, s);
//
//   printf("before summing values");
//   result = _mm_hadd_ps(result, result);
//   result = _mm_hadd_ps(result, result);
//
//   float sum;
//   printf("before returning values");
//   _mm_store_ss(&sum, result);
//   return sum;
// }
