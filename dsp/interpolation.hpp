#pragma once
#ifndef INTERPOLATION_HPP
#define INTERPOLATION_HPP

#ifndef PI
#define PI
const float pi = 3.14159265358979323846;
#endif

namespace dspheaders {

    class Interpolation {
      public:
        static float linear(float position, float* table);
        static float cosine(float position, float* table);
    };

} /* namespace dspheaders */

#endif
