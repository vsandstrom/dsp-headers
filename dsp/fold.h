


// folds a signal back on itself if signal sample crosses min/max threshold
inline float fold(float sample, float max, float min){
  if (sample > max) {
    return sample - (sample - max);
  }
  if (sample < min) {
    return sample + (min + sample);
  }
  return sample;
}

