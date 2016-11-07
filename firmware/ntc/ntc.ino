// https://github.com/giannivh/SmoothThermistor
#include <SmoothThermistor.h>

SmoothThermistor smoothThermistor(
  A0,                                 // the analog pin to read from
  SmoothThermistor::ADC_SIZE_10_BIT,  // the ADC size
  10 * 1000,                          // the nominal resistance
  8030,                               // the series resistance (by measurement)
  3950,                               // the beta coefficient of the thermistor
  25,                                 // the temperature for nominal resistance
  10);                                // the number of samples to take for each measurement

void setup() {
  Serial.begin(115200);
  smoothThermistor.useAREF(true);
}

void loop() {
  auto temperature = smoothThermistor.temperature();
  Serial.print("The sensor temperature is: ");
  Serial.print(temperature);
  Serial.println("*C");
  delay(1000);
}

