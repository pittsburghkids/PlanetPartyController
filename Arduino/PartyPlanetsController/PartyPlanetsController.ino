#include "Globals.h"

const uint8_t boardSelectA = 3;
const uint8_t boardSelectB = 4;

uint8_t board = 0;

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

  // Encoders
  encoderA.begin();
  delay(10);
  encoderB.begin();
  delay(10);
  encoderC.begin();
  delay(10);

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

  // LEDs
  {
    FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(leds, LED_COUNT)
        .setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(LED_BRIGHTNESS);
  }

  // Board select.
  {
    pinMode(boardSelectA, INPUT_PULLUP);
    pinMode(boardSelectB, INPUT_PULLUP);

    int lsb = (digitalRead(boardSelectA) == LOW) ? 1 : 0;
    int msb = (digitalRead(boardSelectB) == LOW) ? 1 : 0;

    board = (msb << 1) | lsb;
  }

  // Board setup.
  switch (board)
  {
  case 0:
    Serial.println("Board: Left");
    setupLeft();
    break;
  case 1:
    Serial.println("Board: Center");
    setupCenter();
    break;
  case 2:
    Serial.println("Board: Right");
    setupRight();
    break;
  default:
    Serial.println("Invalid board selection!");
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
      MIDI.sendNoteOn(60 + i, 127, board + 1);
    }
    else if (buttons[i].released())
    {
      MIDI.sendNoteOff(60 + i, 0, board + 1);
    }
  }

  // Board funtions.
  switch (board)
  {
  case 0:
    loopLeft();
    break;
  case 1:
    loopCenter();
    break;
  case 2:
    loopRight();
    break;
  }

  // LEDs

  FastLED.show();
}
