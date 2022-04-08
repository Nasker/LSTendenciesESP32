#include <Arduino.h>
#include "LSDigitalTrigger.h"

#define BTN_ID 0
#define BTN_PIN 0

LSDigitalTrigger trigger(BTN_ID, BTN_PIN);

void actOnTrigger(int ID, String callbackString){
  Serial.printf("->ID: %d went %s\n", ID, callbackString.c_str());
}

void setup() {
  Serial.begin(115200);
}

void loop() {
  trigger.readnShoot(actOnTrigger);
}