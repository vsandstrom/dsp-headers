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
            float _feedforward;
            float _feedback;
            int _length;
            int _readptr;
            float _prev;
            float _damp;

        public:
            Comb2(float * buffer, int samplerate, int maxsize, float feedback = 0., float feedforward = 0.);
            ~Comb2();
            // Set optional LP damping, [0,1], 0 is off
            void setDamp(float d);
            inline float process(float in);
    };

    inline float Comb2::process(float in) {

  ///
  ///                 allpass filter
  ///
  ///                ╓───> ( * _feedforward )─────────╖
  ///                ║   ╓─────────╖        V
  ///  x(n) ─> ( + )─╨─> ║  z(-M)  ║──╥─> ( + )──> y(n)
  ///            Λ  v(n) ╙─────────╜  ║ 
  ///            ╙────────( * -_feedback ) <─╜
  ///
  ///       where: _feedback == _feedforward


        float v_delayed = _buf[_readptr];
        undenormalise(v_delayed);
        // Simple lowpass filter
        _prev = v_delayed*(1.f-_damp) + _prev*_damp;
        undenormalise(_prev);

        // Feedback part
        float y = in - _feedback*_prev;

        // Feed back feedback part into buffer
        _buf[_readptr] = y;

        // Add feedforward part to output
        float out = _feedforward*y + v_delayed; 

        _readptr = (_readptr + 1) % _maxsize;
        return out;
    }
}


#endif
