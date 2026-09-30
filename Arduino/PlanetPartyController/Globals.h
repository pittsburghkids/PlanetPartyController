#pragma once

#include <Adafruit_TinyUSB.h>
#include <MIDI.h>
#include <FastLED.h>
#include <Bounce2.h>
#include "pio_encoder.h"

#define BOARD_ID_0 2
#define BOARD_ID_1 3
#define BOARD_ID_2 4

#define PARTY_PURPLE CRGB(127, 0, 255)
#define PARTY_WHITE CRGB::White
#define PARTY_DIM CRGB(15, 15, 15)

#define BREATHE_BPM 12
#define BREATHE_MIN 32
#define BREATHE_MAX 255

uint8_t boardId = 0;

// MIDI

Adafruit_USBD_MIDI usb_midi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);

// LEDS

#define LED_DATA_PIN 16
#define LED_BRIGHTNESS 255
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

void fillGauge(CRGBSet &segment, float value, CRGB baseColor, CRGB fillColor)
{
    float position = value * segment.size();
    int whole = (int)position;
    float fraction = position - whole;

    // Fill with the base color.
    segment = baseColor;

    // Fill "whole" part of the gauge.
    for (int i = 0; i < whole && i < segment.size(); i++)
    {
        segment[i] = fillColor;
    }

    // Fill "fraction" part of the gauge.
    if (whole < segment.size())
    {
        segment[whole] = blend(baseColor, fillColor, uint8_t(fraction * 255));
    }
}
