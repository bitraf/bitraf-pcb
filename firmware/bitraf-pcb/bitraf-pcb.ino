#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3f, 2, 1, 0, 4, 5, 6, 7);

static const char* pressText[] = {
  "really quick press!",
  "short press",
  "normal press",
  "long press",
  "very looooooooong press"
};

// https://wiki.nottinghack.org.uk/wiki/Project:Arduino_Rotary_Encoder_Menu_System
template<typename value_t = uint16_t>
class RotaryEncoderDecoder {
  public:
    enum class duration_type : uint8_t { reallyQuickPress, shortPress, normalPress, longPress, veryLongPress };
    using on_click_callback_t = void(*)(duration_type, uint16_t click_length, value_t value);
    using on_move_callback_t = void(*)(bool dir);

    const int PIN_ENC1 = 2;
    const int PIN_ENC2 = 3;
    const int PIN_ENC_GND = 5;
    const int PIN_BTN = 6;
    const int PIN_BTN_GND = 4;
    const int PIN_LED = 13;

  private:
    boolean moving = false;
    volatile value_t encValue = 0;
    value_t encValueTracking = 1;
    boolean enc1 = false;
    boolean enc2 = false;

    on_move_callback_t on_move_callback;
    on_click_callback_t on_click_callback;
  public:
    RotaryEncoderDecoder(on_move_callback_t on_move_callback, on_click_callback_t on_click_callback) :
      on_move_callback(on_move_callback), on_click_callback(on_click_callback) {
    }

    static const long presses[5] = { 40, 150, 300, 800, 1400 };

    void setup() {
      pinMode(PIN_ENC1, INPUT_PULLUP);
      pinMode(PIN_ENC2, INPUT_PULLUP);
      pinMode(PIN_ENC_GND, OUTPUT);
      pinMode(PIN_BTN, INPUT_PULLUP);
      pinMode(PIN_BTN_GND, OUTPUT);
    }

    inline
    value_t currentValue() {
      return encValue;
    }

    void loop() {
      static unsigned long btnHeld = 0;
      moving = true;
      if (encValueTracking != encValue) {
        bool dir = encValueTracking > encValue;

        // Serial.print("encValueTracking: ");
        // Serial.print(encValueTracking, DEC);
        // Serial.print(", encValue: ");
        // Serial.print(encValue, DEC);
        // Serial.print(", dir=");
        // Serial.println(dir ? ">>>" : "<<<");

        on_move_callback(dir);

        encValueTracking = encValue;
      }
      // Upon button press...
      if ((digitalRead(PIN_BTN) == LOW) && !btnHeld) {
        btnHeld = millis();
        digitalWrite(PIN_LED, HIGH);
        // Serial.print("pressed selecting: ");
        // Serial.println(encValue, DEC);
      }
      // Upon button release...
      if ((digitalRead(PIN_BTN) == HIGH) && btnHeld) {
        long t = millis();
        t -= btnHeld;
        digitalWrite(PIN_LED, LOW);
        auto dur = duration_type::veryLongPress;
        for (int i = 0; i <= static_cast<int>(duration_type::veryLongPress); i++) {
          if (t > presses[i])
            continue;
          dur = static_cast<duration_type>(i);
          break;
        }

        on_click_callback(dur, t, encValue);
        btnHeld = 0;
      }
    }

    void intr1() {
      if (moving)
        delay(1);
      if (digitalRead(PIN_ENC1) == enc1)
        return;
      enc1 = !enc1;
      if (enc1 && !enc2)
        encValue += 1;
      moving = false;
    }

    void intr2() {
      if (moving)
        delay(1);
      if (digitalRead(PIN_ENC2) == enc2)
        return;
      enc2 = !enc2;
      if (enc2 && !enc1)
        encValue -= 1;
      moving = false;
    }
};

using decoder_t = RotaryEncoderDecoder<>;

void rotaryChange(decoder_t::duration_type duration_type, uint16_t dur);
void rotaryMove(bool dir);

static decoder_t rotary(rotaryMove, rotaryChange);

void intrEncChange1() {
  rotary.intr1();
}

void intrEncChange2() {
  rotary.intr2();
}

void setup() {
  Serial.begin(115200);

  rotary.setup();
  attachInterrupt(digitalPinToInterrupt(rotary.PIN_ENC1), intrEncChange1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(rotary.PIN_ENC2), intrEncChange2, CHANGE);

  lcd.begin(16, 2);
  lcd.setBacklightPin(3, POSITIVE);

  lcd.setBacklight(HIGH);
  lcd.home();
  lcd.print("Bitraf");
  lcd.setCursor(0, 1);
  lcd.print("             PCB");
}

void loop() {
  rotary.loop();
}

void rotaryMove(bool dir) {
  Serial.println(dir ? ">>>" : "<<<");
}

void rotaryChange(decoder_t::duration_type duration_type, uint16_t duration) {
  Serial.print("released selecting: ");
  Serial.print(rotary.currentValue(), DEC);
  Serial.print(" (after ");
  Serial.print(duration);
  Serial.print(" ms = ");
  Serial.print(pressText[static_cast<int>(duration_type)]);
  Serial.println(")");
}

