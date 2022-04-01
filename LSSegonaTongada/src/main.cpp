#include <Arduino.h>

#define LDR_PIN 13
#define BTN_PIN 0
#define LED_PIN 17

int pwmChannel = 0;
int pwmFrequency = 1000;
int pwmBitResolution = 8;

void setup() {
  Serial.begin(115200);
  pinMode(BTN_PIN, INPUT);
  //pinMode(LED_PIN, OUTPUT);
  ledcSetup(pwmChannel, pwmFrequency, pwmBitResolution); 
  ledcAttachPin(LED_PIN, pwmChannel);

}

void loop() {
  int ldrValue = analogRead(LDR_PIN);
  bool btnValue = !digitalRead(BTN_PIN);
  
  //digitalWrite(LED_PIN, btnValue);
  int pwmValue = map(ldrValue, 0, 4095, 255, 0);
  ledcWrite(pwmChannel, pwmValue);

  Serial.printf("-LDR Value: %d  -BTN Value: %d   -PWM Value:%d\n", ldrValue, btnValue, pwmValue);

  //delay(10);
}