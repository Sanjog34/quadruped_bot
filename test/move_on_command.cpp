#include <Arduino.h>
#include <ESP32Servo.h>

Servo hip;
Servo knee;

const int hipPin  = 13;
const int kneePin = 14;

void setup() {
  Serial.begin(9600);

  hip.setPeriodHertz(50);
  knee.setPeriodHertz(50);

  hip.attach(hipPin, 500, 2400);   // attach hip to GPIO 13
  knee.attach(kneePin, 500, 2400); // attach knee to GPIO 14

  hip.write(0);
  knee.write(0);

  Serial.println("ESP32 Hip & Knee Servos Ready");
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();

    switch (c) {
      case 'h': hip.write(90); break;   // move hip to 90°
      case 'H': hip.write(0); break;    // move hip to 0°
      case 'k': knee.write(90); break;  // move knee to 90°
      case 'K': knee.write(0); break;   // move knee to 0°
      case 'o': hip.write(180); knee.write(180); break; // both to 180°
    }
  }
}
