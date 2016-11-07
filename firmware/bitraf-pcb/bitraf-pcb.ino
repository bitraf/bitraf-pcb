#include "RotaryEncoder.h"
#include "utils.h"

#include <LiquidCrystal_PCF8574.h>
// https://github.com/giannivh/SmoothThermistor
#include <SmoothThermistor.h>

LiquidCrystal_PCF8574 lcd(0x3F);

SmoothThermistor ntc(
  A0,                                 // the analog pin to read from
  SmoothThermistor::ADC_SIZE_10_BIT,  // the ADC size
  10 * 1000,                          // the nominal resistance
  8030,                               // the series resistance (by measurement)
  3950,                               // the beta coefficient of the thermistor
  25,                                 // the temperature for nominal resistance
  10);                                // the number of samples to take for each measurement

static const char* pressText[] = {
  "really quick press!",
  "short press",
  "normal press",
  "long press",
  "very looooooooong press"
};

using decoder_t = RotaryEncoderDecoder<
  2,  // enc 1
  3,  // enc 2
  4>; // button

void rotaryChange(duration_type duration_type, uint16_t dur);
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
  attachInterrupt(digitalPinToInterrupt(decoder_t::PIN_ENC1), intrEncChange1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(decoder_t::PIN_ENC2), intrEncChange2, CHANGE);

  lcd.begin(16, 2);
  lcd.setBacklight(255);

  lcd.home();
  lcd.print("Bitraf       PCB");

  ntc.useAREF(true);
}

void loop() {
  static fixed_interval_timer<1000> temp_timer;
  rotary.loop();
/**/
  if (temp_timer.expired()) {
    auto temperature = ntc.temperature();

    lcd.setCursor(0, 0);
    lcd.print("Bitraf       PCB");
    lcd.setCursor(0, 1);
    lcd.print("Temp            ");
    lcd.setCursor(6, 1);
    lcd.print(temperature);

    Serial.print("Temp: ");
    Serial.println(temperature);
  }
  /**/
}

void rotaryMove(bool dir) {
  Serial.println(dir ? "   >>>" : "<<<");
}

void rotaryChange(duration_type duration_type, uint16_t duration) {
  Serial.print("released selecting: ");
  Serial.print(rotary.currentValue(), DEC);
  Serial.print(" (after ");
  Serial.print(duration);
  Serial.print(" ms = ");
  Serial.print(pressText[static_cast<int>(duration_type)]);
  Serial.println(")");
}

