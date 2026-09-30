#include "Globals.h"

#define LEVER_LED_COUNT 8

PioEncoder *leverEncoder = &encoderOne;
CRGBSet leverSegment = leds(0, LEVER_LED_COUNT - 1);

int leverMax = INT_MIN;
int leverMin = INT_MAX;
int lastValue = 0;

void setupLever()
{
  leverSegment = CRGB::White;
}

void loopLever()
{
  int value = leverEncoder->getCount();
  if (value != lastValue)
  {
    // Update min/max.
    if (value > leverMax)
      leverMax = value;
    else if (value < leverMin)
      leverMin = value;

    // Send normalized value as MIDI CC.
    float normalizedValue = (leverMax == leverMin) ? 0.0f : (float)(value - leverMin) / (float)(leverMax - leverMin);
    MIDI.sendControlChange(20, (1 - normalizedValue) * 127, boardId + 1);

    // Update LED segment.
    fillGauge(leverSegment, 1 - normalizedValue, PARTY_DIM, PARTY_PURPLE);

    lastValue = value;
  }
}
