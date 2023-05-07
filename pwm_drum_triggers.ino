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

// 40kHz. This needs to be high so that the LPF on the PWM outputs can have a
// very sharp attack, which the drum module needs.
#define PWM_FREQUENCY 40000.0f

// output pin configurations

#define VOICE_COUNT 5

// uint8_t PWM_PINS[] = {
//     6,  // bass drum
//     11, // snare drum
//     10, // tom 3 (low tom)
//     5,  // tom 2 (mid tom)
//     12, // tom 1 (high tom)
// };
// uint8_t PWM_NOTES[] = {
//     36, // bass drum, C1
//     40, // electric snare, E1
//     43, // high floor tom, G1
//     47, // low mid tom, B1
//     50, // high tom, D2
// };

// uint8_t PWM_ALTERNATIVE_NOTES[] = {
//     35, // acoustic bass drum, B0
//     38, // acoustic snare, E1
//     41, // low floor tom, G1
//     45, // low tom, A1
//     48, // high mid tom, C2
// };

SAMD_PWM *PWM_Instance;
// SAMD_PWM PWM_Instance(6, PWM_FREQUENCY, 0);
// SAMD_PWM *PWM_Instance[VOICE_COUNT];

MIDI_CREATE_DEFAULT_INSTANCE();

void set_pwm(byte pin, byte percentage) {
  PWM_Instance->setPWM_manual(pin, convertDutyCyclePercentToInt(percentage));
}

// output pin, primary MIDI note, secondary MIDI note
Voice VOICE1(6, 36, 35, set_pwm);
Voice VOICE2(11, 40, 38, set_pwm);
Voice VOICE3(10, 43, 41, set_pwm);
Voice VOICE4(5, 47, 45, set_pwm);
Voice VOICE5(12, 50, 48, set_pwm);

Voice *VOICES[] = {&VOICE1, &VOICE2, &VOICE3, &VOICE4, &VOICE5};

void handleNoteOn(byte channel, byte pitch, byte velocity) {
  if (channel != MIDI_CHANNEL) {
    return;
  }

  for (uint8_t voice_index = 0; voice_index < VOICE_COUNT; voice_index++) {
    if (VOICES[voice_index] && (VOICES[voice_index]->primary_note == pitch ||
                                VOICES[voice_index]->secondary_note == pitch)) {
      ledOn();
      VOICES[voice_index]->noteOn(velocity);
    }
  }
}

void setup() {
  PWM_Instance = new SAMD_PWM(6, PWM_FREQUENCY, 0);
  // for (uint8_t index = 0; index < VOICE_COUNT; index++) {
  //   PWM_Instance[index] = new SAMD_PWM(PWM_PINS[index], PWM_FREQUENCY, 0);
  //   if (PWM_Instance[index]) {
  //     PWM_Instance[index]->setPWM();
  //   }
  // }
  MIDI.begin(MIDI_CHANNEL);
  MIDI.setHandleNoteOn(handleNoteOn);
}

void loop() {
  MIDI.read();

  for (uint8_t voice_index = 0; voice_index < VOICE_COUNT; voice_index++) {
    if (VOICES[voice_index]->tick()) {
      if (LED_IS_ON) {
        ledOff();
      }
      // PWM_Instance[voice_index]->setPWM_manual(PWM_PINS[voice_index], 0);
    }
  }
}
