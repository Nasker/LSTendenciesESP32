#include <Arduino.h>

#define LDR_PIN 13
#define BTN_PIN 0
#define PWM_CHANNEL 0
#define PWM_RESOLUTION 8
#define PWM_FREQUENCY 1000
#define LED_PIN 17
#define BUILTIN_LED 2

#define THRESHOLD 4095/2

void setup() {
  Serial.begin(115200);
  pinMode(BTN_PIN, INPUT_PULLUP);
  ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(LED_PIN, PWM_CHANNEL);
  pinMode(BUILTIN_LED, OUTPUT);
}

void loop() {
  int ldrValue = analogRead(LDR_PIN);
  bool btnValue = !digitalRead(BTN_PIN);
  int ledValue = map(ldrValue, 0, 4095, 255, 0);
  ledcWrite(PWM_CHANNEL, ledValue);
  
  if(ldrValue < THRESHOLD) {
    digitalWrite(BUILTIN_LED, HIGH);
    Serial.println("Light is low");
  } else {
    digitalWrite(BUILTIN_LED, LOW);
    Serial.println("Light is high");
  }

  Serial.printf("-LDR value: %d \t-BTN value %d \t-LED value %d\n", ldrValue, btnValue, ledValue);
  delay(20);
}