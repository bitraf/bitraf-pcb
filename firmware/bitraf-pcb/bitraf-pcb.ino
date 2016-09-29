#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3f, 2, 1, 0, 4, 5, 6, 7);

// https://wiki.nottinghack.org.uk/wiki/Project:Arduino_Rotary_Encoder_Menu_System
namespace encoder {
const int PIN_ENC1 = 3;
const int PIN_ENC2 = 5;
const int PIN_ENC_GND = 4;
const int PIN_BTN = 2;
const int PIN_BTN_GND = 6;
const int PIN_LED = 13;

static boolean moving = false;
volatile unsigned int encValue = 0;
unsigned int encValueTracking = 1;
boolean enc1 = false;
boolean enc2 = false;

// Here I'm messing around with button press durations - we could go to town here!
enum pressDuration { reallyQuickPress, shortPress, normalPress, longPress, veryLongPress };
long presses[] = { 40, 150, 300, 800, 1400 };
char* pressText[] = {"really quick press!", "short press", "normal press", "long press", "very looooooooong press"};
};

using namespace encoder;

void setup() {
  Serial.begin(115200);

  pinMode(PIN_ENC1, INPUT_PULLUP);
  pinMode(PIN_ENC2, INPUT_PULLUP);
  pinMode(PIN_ENC_GND, OUTPUT);
  pinMode(PIN_BTN, INPUT_PULLUP);
  pinMode(PIN_BTN_GND, OUTPUT);

  attachInterrupt(0, intrEncChange1, CHANGE);
  attachInterrupt(1, intrEncChange2, CHANGE);

  lcd.begin(16, 2);
  lcd.setBacklightPin(3, POSITIVE);

  lcd.setBacklight(HIGH);
  lcd.home();
  lcd.print("Bitraf");
  lcd.setCursor(0, 1);
  lcd.print("             PCB");
}

void intrEncChange1() {
  if (moving)
    delay(1);
  if (digitalRead(PIN_ENC1) == enc1)
    return;
  enc1 = !enc1;
  if (enc1 && !enc2)
    encValue += 1;
  moving = false;
}

void intrEncChange2() {
  if (moving)
    delay(1);
  if (digitalRead(PIN_ENC2) == enc2)
    return;
  enc2 = !enc2;
  if (enc2 && !enc1)
    encValue -= 1;
  moving = false;
}

void loop() {
  static unsigned long btnHeld = 0;
  moving = true;
  if (encValueTracking != encValue) {
    Serial.print("encValue: ");
    Serial.println(encValue, DEC);
    encValueTracking = encValue;
  }
  // Upon button press...
  if ((digitalRead(PIN_BTN) == LOW) && !btnHeld) {
    btnHeld = millis();
    digitalWrite(PIN_LED, HIGH);
    Serial.print("pressed selecting: ");
    Serial.println(encValue, DEC);
  }
  // Upon button release...
  if ((digitalRead(PIN_BTN) == HIGH) && btnHeld) {
    long t = millis();
    t -= btnHeld;
    digitalWrite(PIN_LED, LOW);
    int dur = veryLongPress;
    for (int i = 0; i <= veryLongPress; i++) {
      if (t > presses[i])
        continue;
      dur = i;
      break;
    }

    Serial.print("released selecting: ");
    Serial.print(encValue, DEC);
    Serial.print(" (after ");
    Serial.print(t, DEC);
    Serial.print(" ms = ");
    Serial.print(pressText[dur]);
    // Serial.print(dur);
    Serial.println(")");
    btnHeld = 0;
  }
}

