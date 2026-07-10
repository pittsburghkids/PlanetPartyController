#pragma once

#include <Adafruit_TinyUSB.h>
#include <MIDI.h>
#include <FastLED.h>
#include <Bounce2.h>
#include "pio_encoder.h"

// MIDI

Adafruit_USBD_MIDI usb_midi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);

// LEDS

#define LED_DATA_PIN 28
#define LED_BRIGHTNESS 96
#define LED_COUNT 128
CRGBArray<LED_COUNT> leds;

// Buttons.

#define BUTTON_COUNT 6

#define SWITCH_ONE_PIN 9
#define SWITCH_TWO_PIN 10
#define SWITCH_THREE_PIN 11
#define SWITCH_FOUR_PIN 18
#define SWITCH_FIVE_PIN 19
#define SWITCH_SIX_PIN 20

Bounce2::Button buttons[BUTTON_COUNT];

// Encoders.

#define ENCODER_COUNT 3

#define ENCODER_ONE_PIN_A 14
#define ENCODER_ONE_PIN_B 15

#define ENCODER_TWO_PIN_A 12
#define ENCODER_TWO_PIN_B 13

#define ENCODER_THREE_PIN_A 16
#define ENCODER_THREE_PIN_B 17

PioEncoder encoderA(ENCODER_ONE_PIN_A);
PioEncoder encoderB(ENCODER_TWO_PIN_A);
PioEncoder encoderC(ENCODER_THREE_PIN_A);

// Board functions.

extern void setupLeft();
extern void loopLeft();

extern void setupCenter();
extern void loopCenter();

extern void setupRight();
extern void loopRight();
