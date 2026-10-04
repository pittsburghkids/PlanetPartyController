#include "Globals.h"

#define LEVER_LED_COUNT 8

// Percentage to ignore at ends of range.
#define LEVER_DEADZONE 0.025f

// Fast rise / slow fall.
#define LEVER_RISE_TIME 100
#define LEVER_FALL_TIME 750
#define LEVER_SNAP 0.001f

PioEncoder *leverEncoder = &encoderOne;
CRGBSet leverSegment = leds(0, LEVER_LED_COUNT - 1);

int leverMax = INT_MIN;
int leverMin = INT_MAX;
int lastValue = 0;

float currentLevel = 0;
float targetLevel = 0;
unsigned long lastLevelUpdate = 0;

int currentCCValue = 0;
int lastCCValue = 0;

float mapFloat(float value, float fromLow, float fromHigh, float toLow, float toHigh)
{
  return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}

float leverFill(int value)
{
  if (leverMax <= leverMin)
    return 0.0f;

  float deadzone = (leverMax - leverMin) * LEVER_DEADZONE;
  float fill = mapFloat(value, leverMax - deadzone, leverMin + deadzone, 0, 1);

  return constrain(fill, 0.0f, 1.0f);
}

void setupLever()
{
  leverSegment = CRGB::White;
}

void loopLever()
{
  int value = leverEncoder->getCount();

  // Only update when the lever moves.
  if (value != lastValue)
  {
    leverMax = max(leverMax, value);
    leverMin = min(leverMin, value);

    targetLevel = leverFill(value);

    lastValue = value;
  }

  unsigned long now = millis();
  float dt = now - lastLevelUpdate;
  lastLevelUpdate = now;

  // Lerp toward the target.
  if (targetLevel != currentLevel)
  {

    if (targetLevel > currentLevel)
      currentLevel += (targetLevel - currentLevel) * dt / LEVER_RISE_TIME;
    else
      currentLevel += (targetLevel - currentLevel) * dt / LEVER_FALL_TIME;

    // Snap when close.
    if (fabs(targetLevel - currentLevel) < LEVER_SNAP)
      currentLevel = targetLevel;

    currentLevel = constrain(currentLevel, 0.0f, 1.0f);

    // Send normalized value as MIDI CC.
    currentCCValue = (int)(currentLevel * 127);
    if (currentCCValue != lastCCValue)
    {
      MIDI.sendControlChange(20, currentCCValue, boardId + 1);
      lastCCValue = currentCCValue;
    }
  }

  // LED update.
  if (currentLevel > 0)
  {
    partyGauge(leverSegment, currentLevel);
  }
  else
  {
    leverSegment = PARTY_WHITE;
  }
}
