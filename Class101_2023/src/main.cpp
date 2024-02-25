#include <Arduino.h>

#define LED_PIN 17
#define LDR_PIN 13
#define PWM_CH 0
#define PWM_FREQ 1000
#define PWM_RES 8
#define LIGHT_THRESHOLD 2048
#define RELAY_PIN 21
#define BTN_PIN 0


void setup() {
  Serial.begin(115200);
  ledcSetup(PWM_CH,PWM_FREQ, PWM_RES);
  ledcAttachPin(LED_PIN, PWM_CH);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
}

void loop() {
  int ldrRead = analogRead(LDR_PIN);
  Serial.printf("LDR VALUE: %d\n\r", ldrRead);
  int pwmValue = map(ldrRead, 0, 4095, 0, 255);
  ledcWrite(PWM_CH, pwmValue);
  bool btnValue = !digitalRead(BTN_PIN);
  digitalWrite(RELAY_PIN, btnValue);
}