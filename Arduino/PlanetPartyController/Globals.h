#pragma once

#include <Adafruit_TinyUSB.h>
#include <MIDI.h>
#include <FastLED.h>
#include <Bounce2.h>
#include "pio_encoder.h"

#define BOARD_ID_0 2
#define BOARD_ID_1 3
#define BOARD_ID_2 4

uint8_t boardId = 0;

// MIDI

Adafruit_USBD_MIDI usb_midi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);

// LEDS

#define LED_DATA_PIN 16
#define LED_BRIGHTNESS 96
#define LED_COUNT 128
CRGBArray<LED_COUNT> leds;

// Buttons.

#define BUTTON_COUNT 6

#define SWITCH_ONE_PIN 10
#define SWITCH_TWO_PIN 11
#define SWITCH_THREE_PIN 12
#define SWITCH_FOUR_PIN 13
#define SWITCH_FIVE_PIN 14
#define SWITCH_SIX_PIN 15

Bounce2::Button buttons[BUTTON_COUNT];

// Encoders.

#define ENCODER_COUNT 3

#define ENCODER_ONE_PIN_A 21
#define ENCODER_ONE_PIN_B 20

#define ENCODER_TWO_PIN_A 19
#define ENCODER_TWO_PIN_B 18

PioEncoder encoderOne(ENCODER_ONE_PIN_B);
PioEncoder encoderTwo(ENCODER_TWO_PIN_B);

// Board functions.

extern void setupPegs();
extern void loopPegs();

extern void setupCenter();
extern void loopCenter();

extern void setupLever();
extern void loopLever();
