#include <Arduino.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include <AceButton.h>
#include "ProgressStore.hpp"
#include <ArduinoJson.h>
#include <ArduinoOTA.h>

using namespace ace_button;

const int BUTTON_PIN = 0;
const int LED_PIN = LED_BUILTIN;

AceButton button(BUTTON_PIN);
ProgressStore progressStore;

// Replace with your network credentials
const char* ssid = "CLOTENCSACOLLBATO";
const char* password = "Xmp13051985!";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void handleEvent(AceButton*, uint8_t, uint8_t);

String serializeProgress(){
  JsonDocument doc;
  doc["value"] = MAX_PROGRESS - progressStore.getProgress().value;
  doc["percentage"] = progressStore.getProgress().percentage;
  doc["phrase"] = progressStore.getProgress().message;
  String json;
  doc.shrinkToFit();
  serializeJson(doc, json);
  return json;
}

void notifyClients() {
  ws.textAll(serializeProgress());
}

void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("WebSocket client connected: %u\n", client->id());
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("WebSocket client disconnected: %u\n", client->id());
  }
}

void setup(){
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  ButtonConfig* buttonConfig = button.getButtonConfig();
  buttonConfig->setEventHandler(handleEvent);
  buttonConfig->setFeature(ButtonConfig::kFeatureClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureDoubleClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  WiFi.begin(ssid, password);
  //make ip static at 192.168.1.96
  WiFi.config(IPAddress(192, 168, 1, 96), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
  Serial.println("Connecting to WiFi.");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(".");
    delay(1000);
  }

  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/site/index.html", String(), false);
  });
  
  // Route to load style.css file
  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/site/styles.css", "text/css");
  });

  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/site/script.js", "text/javascript");
  });

  server.on("/background_music.mp3", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/site/background_music.mp3", "audio/mpeg");
  });

  server.on("/get-progress", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "application/json", serializeProgress());
  });

  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);

  ArduinoOTA.setPassword("unaipeca");
  ArduinoOTA.begin();

  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(){
  ArduinoOTA.handle();
  button.check();
}

void handleEvent(AceButton*, uint8_t eventType, uint8_t buttonState) {
  switch (eventType) {
    case AceButton::kEventClicked:
      Serial.printf("Current progress: %d\n", progressStore.getProgress().value);
      notifyClients();
      break;
    case AceButton::kEventDoubleClicked:
      progressStore.incrementProgress();
      progressStore.saveProgress();
      Serial.printf("+1, Current progress: %d\n", progressStore.getProgress().value);
      notifyClients();
      break;
    case AceButton::kEventLongPressed:
      progressStore.decrementProgress();
      progressStore.saveProgress();
      Serial.printf("-1, Current progress: %d\n", progressStore.getProgress().value);
      notifyClients();
      break;
  }
}