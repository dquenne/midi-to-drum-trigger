#include "Voice.h"

Voice::Voice(uint8_t index) {
  _halt_at = 0;
  _on = false;
  _velocity = 0;
}

void Voice::noteOn(byte velocity) {
  _halt_at = millis() + 2;
  _on = true;
  _velocity = velocity;
}

void Voice::noteOff(byte velocity) {
  _halt_at = 0;
  _on = false;
  _velocity = velocity;
}

bool Voice::tick() {
  if (_on && millis() > _halt_at) {
    noteOff(0);
    return true;
  }
  return false;
}
