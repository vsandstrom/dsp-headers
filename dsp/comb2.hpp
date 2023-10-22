#ifndef _comb2_
#define _comb2_

namespace dspheaders {


    class Comb2 {
        private: 
            int _maxsize;
            int _samplerate;
            float * _buf;
            float _ff;
            float _fb;
            int _length;
            int _readptr;
            float _prev;
            float _damp;

        public:
            Comb2(float * buffer, int samplerate, int maxsize, float fb = 0., float ff = 0.);
            ~Comb2();
            // Set optional LP damping, [0,1], 0 is off
            void setDamp(float d);
            inline float process(float in);
    };

    inline float Comb2::process(float in) {
        float y = _buf[_readptr];
        float yy = -_fb*y + in;
        _prev = yy*(1.f-_damp) + _prev*_damp;

        _buf[_readptr] = y + _ff*_prev;
        //float out = y + _ff*_prev;

        _readptr = (_readptr + 1) % _maxsize;
        return y;
    }
}


#endif
