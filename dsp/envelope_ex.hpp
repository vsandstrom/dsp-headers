


#include <cstddef>
namespace dspheaders {

  struct BreakPoints {
    float* values = nullptr;
    float* durations = nullptr;
    float* curves = nullptr;
  };




  class EnvelopeEX {
    struct M {
      float* buffer;
      size_t len;
      float position;
      float speed;
    } m;

    void generate(BreakPoints &brk, float samplerate) {
      let mut durations = breakpoints.durations.into_iter();
      // let mut curves = curves.into_iter();
      let mut buffer = vec!();
      for p in breakpoints.values.windows(2) {
        let q = f32::abs(p[1] - p[0]);

        if let Some(time) = durations.next() {
          let num_samples = time * samplerate;
          let m = 1.0 / num_samples;

          for i in 0..(num_samples as usize) {

            if let Some(curves) = breakpoints.curves {
              let mut curves = curves.into_iter();
              if let Some(curve) = curves.next() {
                let slope = q * f32::powf(m * i as f32, curve);
                if p[0] > p[1] {
                  buffer.push(p[0] - slope);
                } else {
                  buffer.push(p[0] + slope);
                }
              }
            } else {
              let curve = 1.0;
              let slope = q * f32::powf(m * i as f32, curve);
              if p[0] > p[1] {
                buffer.push(p[0] - slope);
              } else {
                buffer.push(p[0] + slope);
              }
            }
          }
        }
      }
      buffer
    }

  };
    
}
