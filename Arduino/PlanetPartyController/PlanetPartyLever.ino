#include "Globals.h"

#define LEVER_LED_COUNT 8

// Fraction of the range at each end that reads as empty/full. People push and pull the lever
// past where it settles, so leverMin/leverMax overshoot the actual end positions.
#define LEVER_DEADZONE 0.025f

PioEncoder *leverEncoder = &encoderOne;
CRGBSet leverSegment = leds(0, LEVER_LED_COUNT - 1);

int leverMax = INT_MIN;
int leverMin = INT_MAX;
int lastValue = 0;

// 0 at rest, 1 fully pulled, with the dead zones removed and the middle rescaled to 0-1.
float leverFill(int value)
{
  // No range learned yet, so read as empty.
  if (leverMax <= leverMin)
    return 0.0f;

  float normalizedValue = (leverMax == leverMin) ? 0.0f : (float)(value - leverMin) / (float)(leverMax - leverMin);
  float fill = (1 - normalizedValue - LEVER_DEADZONE) / (1 - 2 * LEVER_DEADZONE);
  return constrain(fill, 0.0f, 1.0f);
}

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
    MIDI.sendControlChange(20, leverFill(value) * 127, boardId + 1);

    lastValue = value;
  }

  // LED update.
  {
    float fill = leverFill(value);

    if (fill > 0)
    {
      partyGauge(leverSegment, fill);
    }
    else
    {
      leverSegment = PARTY_WHITE;
    }
  }
}
