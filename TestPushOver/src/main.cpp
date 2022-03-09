#include <Arduino.h>

#include <WiFi.h>
#include "Pushover.h"

void setup() {
	Serial.begin(115200);
	WiFi.begin("Nasker AP", "fuckyou!");
  Serial.println("Connecting to WiFi..");
	while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  } delay(50);
	Serial.println("Connected");
	
	Pushover po = Pushover("ame2hggh1mxjk52mmvakiwts1k8wyz","unzod5y5ynqjrdywft5i1x9bxqtibf", UNSAFE);
	po.setDevice("NaskerP9");
	po.setMessage("Testnotification2");
	po.setSound("bike");
	Serial.println(po.send()); //should return 1 on success
}

void loop() {
}