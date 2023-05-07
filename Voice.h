#ifndef Voice_h
#define Voice_h

#include <Arduino.h>

class Voice {
public:
  Voice(byte _output_pin, byte _primary_note, byte _secondary_note,
        void (*set_output_percentage)(byte, byte));
  void noteOn(byte velocity);
  void noteOff(byte velocity);
  bool tick();
  byte output_pin;
  byte primary_note;
  byte secondary_note;

private:
  void (*_set_output_percentage)(byte pin, byte percentage);
  unsigned long _halt_at;
  bool _on;
  byte _velocity;
};

#endif
