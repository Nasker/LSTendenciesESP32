#include <Arduino.h>

#define LED_PIN 17
#define LDR_PIN 13
#define PWM_CH 0
#define FREQ 1000
#define RES 12


const int threshold = 2000;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  ledcSetup(PWM_CH, FREQ, RES);
  ledcAttachPin(LED_PIN, PWM_CH);
}

void loop() {
  int ldrRead = analogRead(LDR_PIN);
  int pwmValue = map(ldrRead, 0, 4095, 4095, 0);
  Serial.println(pwmValue);
  ledcWrite(PWM_CH, pwmValue);
}