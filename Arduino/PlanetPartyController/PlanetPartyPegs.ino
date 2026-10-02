#include "Globals.h"

#define PEG_COUNT 6
#define PEG_INSERT_DURATION 16000

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
      segment = breathe(boardId * 8 + i);
    }
    else if (peg->state == Draining)
    {
      float t = (millis() - peg->insertTime) / (float)PEG_INSERT_DURATION;

      if (t <= 1)
      {
        partyGauge(segment, 1 - t);
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