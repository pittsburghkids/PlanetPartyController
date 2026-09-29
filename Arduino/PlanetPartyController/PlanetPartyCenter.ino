#include "Globals.h"

#define CENTER_BUTTON_COUNT 4
#define HOLD_DURATION 3000
#define CENTER_LED_COUNT 24

#define SEND_INTERVAL 1

int lastKnobOneValue = 0;
int lastKnobTwoValue = 0;

int lastSend = 0;

CRGBSet segment = leds(0, CENTER_LED_COUNT - 1);

PioEncoder *knobEncoderOne = &encoderOne;
PioEncoder *knobEncoderTwo = &encoderTwo;

bool holding = false;
unsigned long startTime = 0;

void setupCenter()
{
}

void loopCenter()
{
  for (int i = 0; i < CENTER_BUTTON_COUNT; i++)
  {
    if (buttons[i].pressed())
    {
      MIDI.sendNoteOn(60 + i, 127, boardId + 1);
    }
    else if (buttons[i].released())
    {
      MIDI.sendNoteOff(60 + i, 0, boardId + 1);
    }
  }

  if (buttons[3].pressed())
  {
    holding = true;
    startTime = millis();
  }
  else if (buttons[3].released())
  {
    holding = false;
  }

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

  if (holding)
  {
    float t = (millis() - startTime) / (float)HOLD_DURATION;
    t = constrain(t, 0.0f, 1.0f);

    fillGauge(segment, t, CRGB::White, PARTY_PURPLE);
  }
  else
  {
    segment = CRGB::White;
  }
}