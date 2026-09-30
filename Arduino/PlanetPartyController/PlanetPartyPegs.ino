#include "Globals.h"

#define PEG_COUNT 6
#define PEG_INSERT_DURATION 16000

#define PEG_FLASH_BPM 120
#define PEG_FLASH_MIN 32
#define PEG_FLASH_MAX 128

// Drained: two purple pulses timed from the end of the drain, then dim.
#define PEG_DRAINED_PULSES 2
#define PEG_DRAINED_FADE (PEG_DRAINED_PULSES * 60000UL / PEG_FLASH_BPM)

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
      segment = PARTY_WHITE;
      segment.nscale8(beatsin8(BREATHE_BPM, BREATHE_MIN, BREATHE_MAX));
    }
    else if (peg->state == Draining)
    {
      float t = (millis() - peg->insertTime) / (float)PEG_INSERT_DURATION;

      if (t <= 1)
      {
        fillGauge(segment, 1 - t, PARTY_DIM, PARTY_PURPLE);

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
      unsigned long drainedTime = peg->insertTime + PEG_INSERT_DURATION;
      unsigned long elapsed = millis() - drainedTime;

      if (elapsed < PEG_DRAINED_FADE)
      {
        // Timebase at the drain end and phase 192 (sine trough) so each pulse starts and ends at its dimmest.
        CRGB pulse = PARTY_PURPLE;
        pulse.nscale8(beatsin8(PEG_FLASH_BPM, PEG_FLASH_MIN, PEG_FLASH_MAX, drainedTime, 192));
        segment = pulse;
      }
      else
      {
        segment = PARTY_DIM;
      }
    }
  }
}