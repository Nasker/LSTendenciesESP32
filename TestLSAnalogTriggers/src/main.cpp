#include <Arduino.h>
#include "LSAnalogTrigger.h"

#define N_TRIGGERS 5

#define ID_0 0
#define ID_1 1
#define ID_2 2
#define ID_3 3
#define ID_4 4

#define ANALOG_PIN_0 0
#define ANALOG_PIN_1 1 
#define ANALOG_PIN_2 2
#define ANALOG_PIN_3 3
#define ANALOG_PIN_4 4

const int thresholdValues[N_TRIGGERS] {1000,1000,1000,1000,1000};

LSAnalogTrigger analogTriggers[N_TRIGGERS] =  {
  LSAnalogTrigger(ID_0, ANALOG_PIN_0),
  LSAnalogTrigger(ID_1, ANALOG_PIN_1),
  LSAnalogTrigger(ID_2, ANALOG_PIN_2),
  LSAnalogTrigger(ID_3, ANALOG_PIN_3),
  LSAnalogTrigger(ID_4, ANALOG_PIN_4)
};

void actOnTrigger(int ID, String callbackString){
  Serial.printf("ID: %d, %s\n", ID, callbackString.c_str());
  /*
  Whatever you want to do when the trigger is fired
  */
}

void setup() {
  Serial.begin(115200);
  for(int i=0; i<N_TRIGGERS; i++)
    analogTriggers[i].setThreshold(thresholdValues[i]);
}

void loop() {
  for(LSAnalogTrigger trigger:analogTriggers){
    trigger.readnShoot(actOnTrigger);
  }
}