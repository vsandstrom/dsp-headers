#include "comb2.hpp"

namespace dspheaders {

    Comb2::Comb2(float *buffer, int samplerate, int maxsize, float feedback, float feedforward) {
        _maxsize = maxsize;
        _samplerate = samplerate;
        _feedforward = feedforward;
        _feedback = feedback;
        _readptr = 0.;
        _buf = buffer;
        _prev = 0.;
        _damp = 0.;
    }

    void Comb2::setDamp(float d) {
        _damp = d;
    }

    Comb2::~Comb2() {};


}
