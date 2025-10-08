#include <cmath>
#include <cstddef>
#include <limits>
#include <utility>
#include <stddef.h>

enum Reset {
  SOFT,
  HARD,
};

enum EnvStage {
  ATK,
  DEC,
  SUS,
  REL
};

class ADSREnvelope {
  struct M{
    float atk_value;
    float atk_duration;
    float atk_curve;

    float dec_duration;
    float dec_curve;

    float sus_value;

    float rel_duration;
    float rel_curve;

    EnvStage stage;
    float start;
    float prev;
    float next;
    bool playing;
    Reset reset;
    size_t count;
    float samplerate;
  } self;

  explicit ADSREnvelope(M self) : self(std::move(self)) {}

  // set samplerate to any Update rate if you want it to run  at a slower
  // ''control rate''
  static ADSREnvelope init(float samplerate) {
    return ADSREnvelope(
      M{
        .atk_value = 1.f,
        .atk_duration = 1.f,
        .atk_curve = 0.2f,
        .dec_duration = 0.5f,
        .dec_curve = 0.8f,
        .sus_value = 0.5f,
        .rel_duration = 2.f,
        .rel_curve = 0.8f,
        .stage = EnvStage::ATK,
        .start = std::numeric_limits<float>::epsilon(),
        .prev = std::numeric_limits<float>::epsilon(),
        .next = std::numeric_limits<float>::epsilon(),
        .playing = false,
        .reset = Reset::SOFT,
        .count = 0,
        .samplerate = samplerate
      }
    );
  }

  float play(bool sustain) {
    if (!self.playing) { return 0.f; }
    handle_sustain(sustain);
    float env = calculate_envelope();
    self.next = env;
    return env;
  }


  float calculate_envelope() {
    switch (self.stage) {
      case EnvStage::ATK: {
                            self.count++;
                            return next_value(
                              self.start,
                              self.atk_value,
                              self.atk_duration,
                              self.atk_curve,
                              self.stage,
                              self.count
                            );
                          }
      case EnvStage::DEC: {
                            self.count++;
                            return next_value(
                              self.prev,
                              self.sus_value,
                              self.dec_duration,
                              self.dec_curve,
                              self.stage,
                              self.count
                            );
                          }
      case EnvStage::SUS: {
                            return self.prev;
                          }
      case EnvStage::REL: {
                            self.count++;
                            return next_value(
                              self.prev,
                              std::numeric_limits<float>::epsilon(),
                              self.rel_duration,
                              self.rel_curve,
                              self.stage,
                              self.count
                            );
                          }
    }
  }
  
  inline float next_value(
      float start,
      float end,
      float dur,
      float curve,
      EnvStage stage,
      size_t count
    ) {
    float env = process(start, end, dur, curve, count);
    if (count >= static_cast<size_t>(dur * self.samplerate)) {
      switch (stage) {
        case EnvStage::ATK: { stage = EnvStage::DEC; }
        case EnvStage::DEC: { stage = EnvStage::SUS; }
        case EnvStage::REL: { stage = EnvStage::ATK; 
                              self.playing = false; }
        default: {}
      }
      self.count = 0;
      self.prev = env;
    }
    return env;
  }

  float process(float start, float end, float dur, float curve, size_t count) {
    float t = static_cast<float>(count) / (dur * self.samplerate);
    if (start > end) { return start - std::powf(t, curve) * (start - end); }
    else             { return start + std::powf(t, curve) * (end - start); }
  }
  
  inline void handle_sustain(bool sustain) {
    if (!sustain) {
      switch (self.stage) {
        case EnvStage::REL: {}
        default: {
                   self.stage = EnvStage::REL;
                   self.prev = self.next;
                   self.count = 0;
                 }
      }
    }
  }
  
  void set_attack_val (float atk) { self.atk_value    = atk; }
  void set_attack_dur (float dur) { self.atk_duration = dur; }
  void set_attack_cur (float cur) { self.atk_curve    = cur; }
  void set_decay_dur  (float dur) { self.dec_duration = dur; }
  void set_decay_cur  (float cur) { self.dec_curve    = cur; }
  void set_sustain_val(float sus) { self.sus_value    = sus; }
  void set_release_dur(float dur) { self.rel_duration = dur; }
  void set_release_cur(float cur) { self.rel_curve    = cur; }
  void set_reset_type (Reset rst) { self.reset        = rst; }
  void set_samplerate (float smp) { self.samplerate   = smp; }
};


