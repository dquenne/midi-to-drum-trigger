#include "SAMD_PWM.h"
#include "Voice.h"
#include <MIDI.h>

#define LED_PIN 13
int LED_IS_ON = 0;

void ledOn() {
  digitalWrite(LED_PIN, HIGH);
  LED_IS_ON = 1;
}

void ledOff() {
  digitalWrite(LED_PIN, LOW);
  LED_IS_ON = 0;
}

// standard MIDI channel for drums
#define MIDI_CHANNEL 10

#define convertDutyCyclePercentToInt(percentage) ((percentage * 65536) / 100)
#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)
#define convertVelocityToDutyCycle(velocity) (MIN(MAX(velocity, 20) - 7, 99))

// 40kHz. This needs to be high so that the LPF on the PWM outputs can have a
// very sharp attack, which the drum module needs.
#define PWM_FREQUENCY 40000.0f

// output pin configurations

#define PWM_PIN_COUNT 5

uint8_t PWM_PINS[] = {
    6,  // bass drum
    11, // snare drum
    10, // tom 3 (low tom)
    5,  // tom 2 (mid tom)
    12, // tom 1 (high tom)
};
uint8_t PWM_NOTES[] = {
    36, // bass drum, C1
    40, // electric snare, E1
    43, // high floor tom, G1
    47, // low mid tom, B1
    50, // high tom, D2
};

uint8_t PWM_ALTERNATIVE_NOTES[] = {
    35, // acoustic bass drum, B0
    38, // acoustic snare, E1
    41, // low floor tom, G1
    45, // low tom, A1
    48, // high mid tom, C2
};

Voice VOICE1(0);
Voice VOICE2(1);
Voice VOICE3(2);
Voice VOICE4(3);
Voice VOICE5(4);

Voice *VOICES[] = {&VOICE1, &VOICE2, &VOICE3, &VOICE4, &VOICE5};

SAMD_PWM *PWM_Instance[PWM_PIN_COUNT];

MIDI_CREATE_DEFAULT_INSTANCE();

void handleNoteOn(byte channel, byte pitch, byte velocity) {
  for (uint8_t pin_index = 0; pin_index < PWM_PIN_COUNT; pin_index++) {
    if (PWM_Instance[pin_index] &&
        (PWM_NOTES[pin_index] == pitch ||
         PWM_ALTERNATIVE_NOTES[pin_index] == pitch)) {
      ledOn();
      PWM_Instance[pin_index]->setPWM_manual(
          PWM_PINS[pin_index],
          convertDutyCyclePercentToInt(convertVelocityToDutyCycle(velocity)));
      VOICES[pin_index]->noteOn(velocity);
    }
  }
}

void setup() {
  for (uint8_t index = 0; index < PWM_PIN_COUNT; index++) {
    PWM_Instance[index] = new SAMD_PWM(PWM_PINS[index], PWM_FREQUENCY, 0);
    if (PWM_Instance[index]) {
      PWM_Instance[index]->setPWM();
    }
  }
  MIDI.begin(MIDI_CHANNEL);
  MIDI.setHandleNoteOn(handleNoteOn);
}

void loop() {
  MIDI.read();

  for (uint8_t pin_index = 0; pin_index < PWM_PIN_COUNT; pin_index++) {
    if (VOICES[pin_index]->tick()) {
      if (LED_IS_ON) {
        ledOff();
      }
      PWM_Instance[pin_index]->setPWM_manual(PWM_PINS[pin_index], 0);
    }
  }
}
