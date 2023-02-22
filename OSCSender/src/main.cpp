#include <Arduino.h>

#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "LSDigitalTrigger.h"

#define BTN_ID 0
#define BTN_PIN 0

//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

char ssid[] = "lstendencieswifi";          // your network SSID (name)
char pass[] = "arduinorules";                    // your network password

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
const IPAddress outIp(192,168,0,102);        // remote IP of your computer
const unsigned int outPort = 9999;          // remote port to receive OSC
const unsigned int localPort = 8888;        // local port to listen for OSC packets (actually not used for sending)

LSDigitalTrigger trigger(BTN_ID, BTN_PIN);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

void printToScreen(String firstLine, String secondLine, String thirdLine, String fourthLine){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(firstLine);
  display.println(secondLine);
  display.println(thirdLine);
  display.println(fourthLine);
  display.display();
}

void actOnTrigger(int ID, String callbackString){
    Serial.println("Button pressed");
    OSCMessage msg("/test");
    msg.add(callbackString);
    Udp.beginPacket(outIp, outPort);
    msg.send(Udp);
    Udp.endPacket();
    msg.empty();
    Serial.println("Sent OSC message");
    printToScreen(WiFi.localIP().toString(), String(localPort),"Sent OSC Message", callbackString);
}

void setup() {
  Serial.begin(115200);
  //initialize OLED
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Connect to WiFi network
  Serial.printf("Connecting to: %s\n", ssid);
  printToScreen("Connecting to:", ssid, "", "");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected to WiFi with IP: ");
  Serial.println(WiFi.localIP());
  Udp.begin(localPort);
  Serial.printf("Local port: %d\n", localPort);
}

void loop() {
  trigger.readnShoot(actOnTrigger);
}