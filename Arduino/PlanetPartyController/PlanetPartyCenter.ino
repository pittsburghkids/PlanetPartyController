#include "Globals.h"

#define CENTER_BUTTON_COUNT 4
#define CENTER_LED_COUNT 24

// Launch meter. Mirrored in PanelButtonMain.cs — keep these in sync.
#define FILL_DURATION 3000   // ms, empty to full while filling
#define REPRESS_WINDOW 400   // ms, keeps filling this long after a release
#define DRAIN_DURATION 3000  // ms, full to empty once not filling
#define FLIGHT_DURATION 3000 // ms, meter on hold after a launch (SceneManager.flightDuration)

#define SEND_INTERVAL 1

int lastKnobOneValue = 0;
int lastKnobTwoValue = 0;

int lastSend = 0;

CRGBSet segment = leds(0, CENTER_LED_COUNT - 1);

PioEncoder *knobEncoderOne = &encoderOne;
PioEncoder *knobEncoderTwo = &encoderTwo;

float meter = 0;
bool waitingForRelease = false;     // held through a flight; doesn't count until let go
unsigned long keepFillingUntil = 0;
unsigned long flyingUntil = 0;  // end of the post-launch hold
unsigned long lastMeterUpdate = 0;

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

  // Launch meter.
  {
    unsigned long now = millis();
    float dt = now - lastMeterUpdate;
    lastMeterUpdate = now;

    bool buttonDown = buttons[3].isPressed();

    if (buttons[3].released())
    {
      if (!waitingForRelease)
        keepFillingUntil = now + REPRESS_WINDOW;
      waitingForRelease = false;
    }

    // On hold while flying. Anything still held on arrival must be released first.
    if ((long)(flyingUntil - now) > 0)
    {
      meter = 0;
      keepFillingUntil = now;
      waitingForRelease = buttonDown;
    }

    bool filling = (buttonDown && !waitingForRelease) || (long)(keepFillingUntil - now) > 0;
    meter += filling ? dt / FILL_DURATION : -dt / DRAIN_DURATION;
    meter = constrain(meter, 0.0f, 1.0f);

    // Launch. The flight hold above resets the meter from the next loop.
    if (meter >= 1.0f)
      flyingUntil = now + FLIGHT_DURATION;
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

  if (meter > 0)
  {
    partyGauge(segment, meter);
  }
  else
  {
    segment = PARTY_WHITE;
  }
}