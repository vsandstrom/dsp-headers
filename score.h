#pragma once
// DURATION OF THE GENERATED TONE
const int DURATION =        30000; // milliseconds
// IF YOUR SOUNDCARD DO NOT FOR SUPPORT 48kHz, CHANGE IT HERE:
const float  SAMPLE_RATE =  48000.f;

// Fundamental pitch
const float FUND = 80.f;
// Pitch score
const float SCORE[5] = {
  FUND,
  FUND * 4/3,
  FUND * 3/2*3,
  FUND * 12/7*2,
  FUND * 3,
};

const float AMP[] = {
  0.55, 0.5, 0.45, 0.3,
  0.5,
  0.55, 0.5, 0.45,
  0.3, 0.4,
  0.55, 0.5, 0.45,
  0.75, 0.65,
};

// Duration before retriggering the envelope, in seconds from start
// float dur[21] = {1.2, 1.4, 2.0, 2.2, 2.6, 3.8, 4.0, 4.1, 4.2, 4.3, 4.7, 5.1, 5.3, 5.6, 5.7, 6.2, 6.4, 7.2, 7.6, 8.0, 8.6};
const float DUR[15] = {
  1.2, 1.2, 1.2, 2.4, 
  3.6, 
  1.2, 1.2, 2.4,
  1.2, 3.6,
  2.4, 1.2, 1.2, 
  3.6, 1.2
};

