#include <Arduino.h>
#include "LSDigitalTrigger.h"
#include "LSAnalogTrigger.h"

#define BTN_ID 0
#define BTN_PIN 0
#define LDR_ID 1
#define LDR_PIN 35

LSDigitalTrigger digitalTrigger(BTN_ID, BTN_PIN);
LSAnalogTrigger analogTrigger(LDR_ID, LDR_PIN);

void actOnTrigger(int ID, String callbackString){
  Serial.printf("->ID: %d went %s\n", ID, callbackString.c_str());
}

void setup() {
  Serial.begin(115200);
  analogTrigger.setThreshold(1000);
}

void loop() {
  digitalTrigger.readnShoot(actOnTrigger);
  analogTrigger.readnShoot(actOnTrigger);
}