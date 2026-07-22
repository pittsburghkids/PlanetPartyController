#include "Globals.h"

#define RIGHT_MIDI_CHANNEL 3
#define LEVER_COUNT 3
#define RIGHT_LED_COUNT 24
struct Lever
{
  PioEncoder *encoder;
  int max = INT_MIN;
  int min = INT_MAX;
  int value = 0;
  int lastValue = 0;

  bool update()
  {
    value = encoder->getCount();
    if (value != lastValue)
    {
      lastValue = value;
      return true;
    }
    return false;
  }

  float getNoramlizedValue()
  {
    if (value > max)
      max = value;
    else if (value < min)
      min = value;

    if (max == min)
      return 0.0f;

    return (float)(value - min) / (float)(max - min);
  }
};

Lever levers[LEVER_COUNT];
CRGBSet leverSegments[LEVER_COUNT] = {
    leds(0, 5),
    leds(6, 11),
    leds(12, 17),
};

void setupRight()
{
  // Initalize levers.
  {
    levers[0].encoder = &encoderA;
    levers[1].encoder = &encoderB;
    levers[2].encoder = &encoderC;
  }
}

void loopRight()
{
  // Serial.println("Encoder A: " + String(encoderA.getCount()) + " Encoder B: " + String(encoderB.getCount()) + " Encoder C: " + String(encoderC.getCount()));
  for (int i = 0; i < LEVER_COUNT; i++)
  {
    //
    if (levers[i].update())
    {
      float value = levers[i].getNoramlizedValue();
      MIDI.sendControlChange(20 + i, value * 127, RIGHT_MIDI_CHANNEL);
    }

    // Reset segment.
    leverSegments[i].fill_solid(CRGB::Black);

    // Illuminate action section.
    {
      int segmentOn = levers[i].getNoramlizedValue() * 6;
      segmentOn = constrain(segmentOn, 0, 5);
      Serial.println("Lever " + String(i) + " value: " + String(levers[i].value) + " normalized: " + String(levers[i].getNoramlizedValue()) + " segmentOn: " + String(segmentOn));

      for (int j = 0; j < segmentOn; j++)
        leverSegments[i][j] = CRGB::White;
    }
  }
}