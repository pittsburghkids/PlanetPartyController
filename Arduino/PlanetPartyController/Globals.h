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
#define PARTY_WHITE CRGB(192, 96, 255)
#define PARTY_DIM (PARTY_WHITE.scale8(16))

// Purple pulse on active gauges, shared by all boards.
#define PEG_FLASH_BPM 120
#define PEG_FLASH_MIN 32
#define PEG_FLASH_MAX 255

// Idle breathing. Each light gets its own pace in this range, plus its own phase.
#define BREATHE_BPM_MIN 14
#define BREATHE_BPM_MAX 20

uint8_t boardId = 0;

// MIDI

Adafruit_USBD_MIDI usb_midi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);

// LEDS

#define LED_DATA_PIN 16
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

// Idle, breathing between dim and white at a pace and phase picked from the seed so nothing breathes in lockstep.
// Seed with boardId * 8 + segment index so it's unique across boards.
CRGB breathe(uint8_t seed)
{
    uint8_t h = seed * 157 + 71; // scatter neighboring seeds

    // accum88 (8.8 fixed point) BPM, so paces fall between whole BPMs and drift apart over time.
    accum88 bpm = (BREATHE_BPM_MIN << 8) + scale16by8((BREATHE_BPM_MAX - BREATHE_BPM_MIN) << 8, h);

    return blend(PARTY_DIM, PARTY_WHITE, beatsin8(bpm, 0, 255, 0, uint8_t(h * 3)));
}

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

// Pulsing purple gauge against a dim background.
void partyGauge(CRGBSet &segment, float fill)
{
    CRGB partyPulse = PARTY_PURPLE;
    partyPulse.nscale8(beatsin8(PEG_FLASH_BPM, PEG_FLASH_MIN, PEG_FLASH_MAX));

    fillGauge(segment, fill, PARTY_DIM, partyPulse);
}
