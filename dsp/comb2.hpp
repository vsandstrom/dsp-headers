#ifndef _denormals_
#define _denormals_

#define undenormalise(sample) if(((*(unsigned int*)&sample)&0x7f800000)==0) sample=0.0f

#endif

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
        undenormalise(y);
        _prev = y*(1.f-_damp) + _prev*_damp;
        undenormalise(_prev);
        float yy = in - _fb*_prev;

        _buf[_readptr] = yy;
        float out = yy + _ff*y; 

        _readptr = (_readptr + 1) % _maxsize;
        return out;
    }
}


#endif
