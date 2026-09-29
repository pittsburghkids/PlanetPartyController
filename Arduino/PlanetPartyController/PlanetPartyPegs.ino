#include "Globals.h"

#define PEG_COUNT 6
#define PEG_INSERT_DURATION 16000

#define PEG_BREATHE_BPM 12
#define PEG_BREATHE_MIN 32
#define PEG_BREATHE_MAX 128

#define PEG_FLASH_BPM 120
#define PEG_FLASH_MIN 32
#define PEG_FLASH_MAX 128

// Drained: very dim white, faded into from the end of the drain.
#define PEG_DRAINED_FADE 1000
#define PEG_DRAINED_COLOR CRGB(8, 8, 8)

CRGBSet segments[PEG_COUNT] = {
    leds(0, 15),
    leds(16, 31),
    leds(32, 47),
    leds(48, 63),
    leds(64, 79),
    leds(80, 95),
};

enum PegState
{
  Idle,
  Draining,
  Empty
};
struct Peg
{
  unsigned long insertTime;
  PegState state;
};

Peg pegs[6];

void setupPegs()
{
}

void loopPegs()
{
  for (int i = 0; i < PEG_COUNT; i++)
  {
    Peg *peg = &pegs[i];
    auto button = buttons[i];
    auto segment = segments[i];

    if (button.pressed())
    {
      peg->insertTime = millis();
      peg->state = Draining;

      MIDI.sendNoteOn(60 + i, 127, boardId + 1);
    }

    if (button.released())
    {
      if (peg->state != Empty)
      {
        MIDI.sendNoteOff(60 + i, 0, boardId + 1);
      }

      peg->state = Idle;
    }

    if (peg->state == Idle)
    {
      // Breathe with a sine wave on the clock.
      segment = CRGB::White;
      segment.nscale8(beatsin8(PEG_BREATHE_BPM, PEG_BREATHE_MIN, PEG_BREATHE_MAX));
    }
    else if (peg->state == Draining)
    {
      float t = (millis() - peg->insertTime) / (float)PEG_INSERT_DURATION;

      if (t <= 1)
      {
        fillGauge(segment, 1 - t, CRGB::White, PARTY_PURPLE);

        // Pulse on the global clock, not insertTime, so all draining pegs beat together.
        segment.nscale8(beatsin8(PEG_FLASH_BPM, PEG_FLASH_MIN, PEG_FLASH_MAX));
      }
      else
      {
        peg->state = Empty;
        MIDI.sendNoteOff(60 + i, 0, boardId + 1);
      }
    }

    if (peg->state == Empty)
    {
      unsigned long elapsed = millis() - peg->insertTime - PEG_INSERT_DURATION;

      if (elapsed < PEG_DRAINED_FADE)
      {
        // Crossfade from the empty gauge, still pulsing on the global clock, down to dim.
        CRGB pulse = CRGB::White;
        pulse.nscale8(beatsin8(PEG_FLASH_BPM, PEG_FLASH_MIN, PEG_FLASH_MAX));
        segment = blend(pulse, PEG_DRAINED_COLOR, ease8InOutQuad(elapsed * 255 / PEG_DRAINED_FADE));
      }
      else
      {
        segment = PEG_DRAINED_COLOR;
      }
    }
  }
}