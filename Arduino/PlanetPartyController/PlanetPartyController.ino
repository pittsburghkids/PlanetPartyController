#include "Globals.h"

void setup()
{
  // Initialize USB.
  {
    if (!TinyUSBDevice.isInitialized())
    {
      TinyUSBDevice.begin(0);
    }
  }

  // Initialize MIDI.
  {
    MIDI.begin(MIDI_CHANNEL_OMNI);

    if (TinyUSBDevice.mounted())
    {
      TinyUSBDevice.detach();
      delay(10);
      TinyUSBDevice.attach();
    }
  }

  // Buttons
  {
    buttons[0].attach(SWITCH_ONE_PIN, INPUT_PULLUP);
    buttons[0].setPressedState(LOW);

    buttons[1].attach(SWITCH_TWO_PIN, INPUT_PULLUP);
    buttons[1].setPressedState(LOW);

    buttons[2].attach(SWITCH_THREE_PIN, INPUT_PULLUP);
    buttons[2].setPressedState(LOW);

    buttons[3].attach(SWITCH_FOUR_PIN, INPUT_PULLUP);
    buttons[3].setPressedState(LOW);

    buttons[4].attach(SWITCH_FIVE_PIN, INPUT_PULLUP);
    buttons[4].setPressedState(LOW);

    buttons[5].attach(SWITCH_SIX_PIN, INPUT_PULLUP);
    buttons[5].setPressedState(LOW);
  }

  // Encoders
  {
    encoderOne.begin();
    encoderTwo.begin();
  }

  // LEDs
  {
    FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(leds, LED_COUNT)
        .setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(LED_BRIGHTNESS);
  }

  // Board select.
  {
    pinMode(BOARD_ID_0, INPUT_PULLUP);
    pinMode(BOARD_ID_1, INPUT_PULLUP);
    pinMode(BOARD_ID_2, INPUT_PULLUP);

    int boardId0 = (digitalRead(BOARD_ID_0) == LOW) ? 1 : 0;
    int boardId1 = (digitalRead(BOARD_ID_1) == LOW) ? 1 : 0;
    int boardId2 = (digitalRead(BOARD_ID_2) == LOW) ? 1 : 0;

    boardId = (boardId2 << 2) | (boardId1 << 1) | boardId0;
  }

  // Board setup.
  switch (boardId)
  {
  case 0:
    setupCenter();
    break;
  case 1:
    setupPegs();
    break;
  default:
    setupLever();
    break;
  }
}

void loop()
{
  // Wait for USB.
  if (!TinyUSBDevice.mounted())
  {
    return;
  }

  // Update buttons.
  for (int i = 0; i < BUTTON_COUNT; i++)
  {
    buttons[i].update();

    if (buttons[i].pressed())
    {
      MIDI.sendNoteOn(60 + i, 127, boardId + 1);
    }
    else if (buttons[i].released())
    {
      MIDI.sendNoteOff(60 + i, 0, boardId + 1);
    }
  }

  // Board funtions.
  switch (boardId)
  {
  case 0:
    loopCenter();
    break;
  case 1:
    loopPegs();
    break;
  default:
    loopLever();
    break;
  }

  // LEDs

  FastLED.show();
}
