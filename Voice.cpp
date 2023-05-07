#include "Voice.h"

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)
#define convertVelocityToDutyCycle(velocity) (MIN(MAX(velocity, 20) - 7, 99))

Voice::Voice(byte _output_pin, byte _primary_note, byte _secondary_note,
             void (*set_output_percentage)(byte, byte)) {
  output_pin = _output_pin;
  primary_note = _primary_note;
  secondary_note = _secondary_note;
  _set_output_percentage = set_output_percentage;
  _halt_at = 0;
  _on = false;
  _velocity = 0;
}

void Voice::noteOn(byte velocity) {
  _set_output_percentage(output_pin, convertVelocityToDutyCycle(velocity));
  _halt_at = millis() + 2;
  _on = true;
  _velocity = velocity;
}

void Voice::noteOff(byte velocity) {
  _set_output_percentage(output_pin, convertVelocityToDutyCycle(0));
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
