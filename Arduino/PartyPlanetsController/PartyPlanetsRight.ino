#include "Globals.h"

#define RIGHT_MIDI_CHANNEL 3
#define LEVER_COUNT 3
#define RIGHT_LED_COUNT 24

CRGBSet rightSegment = leds(0, RIGHT_LED_COUNT - 1);

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
    if (levers[i].update())
    {
      float value = levers[i].getNoramlizedValue();
      MIDI.sendControlChange(20 + i, value * 127, RIGHT_MIDI_CHANNEL);
    }
  }

  rightSegment.fill_solid(CRGB::DarkRed);
}