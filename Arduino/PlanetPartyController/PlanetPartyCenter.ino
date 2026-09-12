#include "Globals.h"

#define CENTER_LED_COUNT 24

#define SEND_INTERVAL 1

int lastKnobOneValue = 0;
int lastKnobTwoValue = 0;

int lastSend = 0;

CRGBSet segment = leds(0, CENTER_LED_COUNT - 1);

PioEncoder *knobEncoderOne = &encoderOne;
PioEncoder *knobEncoderTwo = &encoderTwo;

void setupCenter()
{
}

void loopCenter()
{
  // Rate limit.
  if (millis() - lastSend > SEND_INTERVAL)
  {
    // Encoders
    {
      int knobOneValue = knobEncoderOne->getCount();
      if (knobOneValue != lastKnobOneValue)
      {
        int delta = constrain(knobOneValue - lastKnobOneValue, -63, 63);
        int deltaValue = delta >= 0 ? delta : (128 + delta);

        MIDI.sendControlChange(20, deltaValue, boardId + 1);

        lastKnobOneValue = knobOneValue;
      }

      int knobTwoValue = knobEncoderTwo->getCount();
      if (knobTwoValue != lastKnobTwoValue)
      {
        int delta = constrain(knobTwoValue - lastKnobTwoValue, -63, 63);
        int deltaValue = delta >= 0 ? delta : (128 + delta);

        MIDI.sendControlChange(21, deltaValue, boardId + 1);

        lastKnobTwoValue = knobTwoValue;
      }
    }

    lastSend = millis();
  }

  int counter = (millis() * 2 / 1000) % CENTER_LED_COUNT;

  for (int j = 0; j < segment.size(); j++)
  {
    if (j <= counter)
    {
      segment[j] = CRGB::Purple;
    }
    else
    {
      segment[j] = CRGB::Black;
    }
  }
}