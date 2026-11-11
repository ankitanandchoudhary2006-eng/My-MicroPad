#include <Keyboard.h>

const uint8_t ROWS = 3;
const uint8_t COLS = 3;

const uint8_t rowPins[ROWS] = {2, 3, 4};
const uint8_t colPins[COLS] = {5, 6, 7};


enum Action : uint8_t {
  ACT_COPY,
  ACT_PASTE,
  ACT_CUT,
  ACT_UNDO,
  ACT_SAVE,
  ACT_ALT_TAB,
  ACT_SNIP,
  ACT_TASK_MANAGER,
  ACT_ENTER
};

const Action keyActions[ROWS][COLS] = {
  {ACT_COPY, ACT_PASTE, ACT_CUT},
  {ACT_UNDO, ACT_SAVE,  ACT_ALT_TAB},
  {ACT_SNIP, ACT_TASK_MANAGER, ACT_ENTER}
};

const unsigned long DEBOUNCE_MS = 25;
bool lastReading[ROWS][COLS];
bool stableState[ROWS][COLS];
unsigned long lastChangeMs[ROWS][COLS];

void releaseAllKeys() {
  Keyboard.releaseAll();
}

void sendShortcut(Action action) {
  releaseAllKeys();
  switch (action) {
    case ACT_COPY:
      Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('c');
      delay(40); releaseAllKeys(); break;
    case ACT_PASTE:
      Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('v');
      delay(40); releaseAllKeys(); break;
    case ACT_CUT:
      Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('x');
      delay(40); releaseAllKeys(); break;
    case ACT_UNDO:
      Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('z');
      delay(40); releaseAllKeys(); break;
    case ACT_SAVE:
      Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('s');
      delay(40); releaseAllKeys(); break;
    case ACT_ALT_TAB:
      Keyboard.press(KEY_LEFT_ALT); Keyboard.press(KEY_TAB);
      delay(120); releaseAllKeys(); break;
    case ACT_SNIP:
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press('s');
      delay(60); releaseAllKeys(); break;
    case ACT_TASK_MANAGER:
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press(KEY_ESC);
      delay(60); releaseAllKeys(); break;
    case ACT_ENTER:
      Keyboard.write(KEY_RETURN); break;
  }
}

void setup() {
  // Rows are driven one at a time; inactive rows are high-impedance.
  for (uint8_t r = 0; r < ROWS; r++) {
    pinMode(rowPins[r], INPUT);
  }
  for (uint8_t c = 0; c < COLS; c++) {
    pinMode(colPins[c], INPUT_PULLUP);
  }

  for (uint8_t r = 0; r < ROWS; r++) {
    for (uint8_t c = 0; c < COLS; c++) {
      lastReading[r][c] = false;
      stableState[r][c] = false;
      lastChangeMs[r][c] = 0;
    }
  }

  delay(500);
  Keyboard.begin();
}

void scanMatrix() {
  for (uint8_t r = 0; r < ROWS; r++) {
    // Enable this row and pull it LOW.
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], LOW);
    delayMicroseconds(5);

    for (uint8_t c = 0; c < COLS; c++) {
      // With COL2ROW diodes, a pressed key pulls its column LOW.
      bool pressed = (digitalRead(colPins[c]) == LOW);

      if (pressed != lastReading[r][c]) {
        lastReading[r][c] = pressed;
        lastChangeMs[r][c] = millis();
      }

      if ((millis() - lastChangeMs[r][c]) >= DEBOUNCE_MS &&
          pressed != stableState[r][c]) {
        stableState[r][c] = pressed;
        if (pressed) {
          sendShortcut(keyActions[r][c]);
        }
      }
    }

    // Return this row to high impedance before scanning the next.
    pinMode(rowPins[r], INPUT);
  }
}

void loop() {
  scanMatrix();
}
