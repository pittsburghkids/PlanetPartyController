#include "Globals.h"

#define CENTER_MIDI_CHANNEL 2
#define CENTER_LED_COUNT 24

#define SEND_INTERVAL 1

int lastEncoderAValue = 0;
int lastEncoderBValue = 0;

int lastSend = 0;

CRGBSet segment = leds(0, CENTER_LED_COUNT - 1);

void setupCenter()
{
}

void loopCenter()
{
  // Serial.println("Encoder A: " + String(encoderA.getCount()) + " Encoder B: " + String(encoderB.getCount()));

  // Rate limit.
  if (millis() - lastSend > SEND_INTERVAL)
  {
    // Encoders
    {
      int encoderAValue = encoderA.getCount();
      if (encoderAValue != lastEncoderAValue)
      {
        int delta = constrain(encoderAValue - lastEncoderAValue, -63, 63);
        int deltaValue = delta >= 0 ? delta : (128 + delta);

        MIDI.sendControlChange(20, deltaValue, CENTER_MIDI_CHANNEL);

        lastEncoderAValue = encoderAValue;
      }

      int encoderBValue = encoderB.getCount();
      if (encoderBValue != lastEncoderBValue)
      {
        int delta = constrain(encoderBValue - lastEncoderBValue, -63, 63);
        int deltaValue = delta >= 0 ? delta : (128 + delta);

        MIDI.sendControlChange(21, deltaValue, CENTER_MIDI_CHANNEL);

        lastEncoderBValue = encoderBValue;
      }
    }

    lastSend = millis();
  }

  // segment.fill_solid(CRGB::DarkBlue);

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