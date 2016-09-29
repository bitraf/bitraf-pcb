#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3f, 2, 1, 0, 4, 5, 6, 7);

void setup() {
  Serial.begin(115200);

  lcd.begin(16, 2);
  lcd.setBacklightPin(3, POSITIVE);

  lcd.setBacklight(HIGH);
  lcd.home();
  lcd.print("Bitraf");
  lcd.setCursor(0, 1);
  lcd.print("             PCB");
}

void loop() {
}

