#include <Arduino.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include <AceButton.h>
#include "ProgressStore.hpp"

using namespace ace_button;

const int BUTTON_PIN = 0;
const int LED_PIN = LED_BUILTIN;

AceButton button(BUTTON_PIN);
ProgressStore progressStore;
// Replace with your network credentials
const char* ssid = "CLOTENCSACOLLBATO";
const char* password = "Xmp13051985!";

String ledState;
AsyncWebServer server(80);

void handleEvent(AceButton*, uint8_t, uint8_t);
 
void setup(){
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  ButtonConfig* buttonConfig = button.getButtonConfig();
  buttonConfig->setEventHandler(handleEvent);
  buttonConfig->setFeature(ButtonConfig::kFeatureDoubleClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false);
  });
  
  // Route to load style.css file
  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/styles.css", "text/css");
  });

  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/script.js", "text/javascript");
  });

  server.on("/cursor1.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/cursor.png", "image/png");
  });

  server.on("/cursor2.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/cursor.png", "image/png");
  });

  server.on("/goal.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/goal.png", "image/png");
  });

  server.on("/background.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/background.png", "image/png");
  });

    server.on("/background_music.mp3", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/background_music.mp3", "audio/mpeg");
  });

  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(){
  button.check();
}

void handleEvent(AceButton*, uint8_t eventType, uint8_t buttonState) {
  switch (eventType) {
    case AceButton::kEventDoubleClicked:
      progressStore.incrementProgress();
      Serial.printf("Current progress: %d\n", progressStore.getProgress());
      break;
    case AceButton::kEventLongPressed:
      progressStore.resetProgress();
      Serial.println("Resetting progress");
      break;
  }
}