#ifndef Voice_h
#define Voice_h

#include <Arduino.h>

class Voice {
public:
  Voice(uint8_t index);
  void noteOn(uint8_t velocity);
  void noteOff(uint8_t velocity);
  bool tick();

private:
  unsigned long _halt_at;
  bool _on;
  uint8_t _velocity;
};

#endif
