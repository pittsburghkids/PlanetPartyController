#include "Globals.h"

#define LEFT_MIDI_CHANNEL 1

CRGBSet segments[BUTTON_COUNT] = {
    leds(32, 47),
    leds(16, 31),
    leds(0, 15),
    leds(80, 95),
    leds(64, 79),
    leds(48, 63),
};

void setupLeft()
{
}

void loopLeft()
{
  for (int i = 0; i < BUTTON_COUNT; i++)
  {
    auto button = buttons[i];
    auto segment = segments[i];

    // LED state.
    if (buttons[i].isPressed())
    {
      uint16_t offset = (millis() / 32) % segment.size();

      for (int j = 0; j < segment.size(); j++)
      {
        int idx = (j + offset) % segment.size();
        float t = float(idx) / (segment.size() - 1);
        segment[j] = CRGB::Purple;
        segment[j].fadeToBlackBy(t * 255);
      }
    }
    else
    {
      segment.fill_solid(CRGB::Gray25);
    }
  }
}