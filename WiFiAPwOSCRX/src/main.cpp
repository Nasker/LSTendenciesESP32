#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "WiFiUdp.h"
#include <OSCBundle.h>
#include "WiFi.h"

WiFiUDP Udp;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 16
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_ADDRESS 0x3C

const char* ssid = "WIFIGRATIS!";
const char* pwd = "12345678";
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

IPAddress ip(192, 168, 4, 1);
IPAddress outIp(192, 168, 4, 2);
const unsigned int outPort = 9999;

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

void setup() {
  Serial.begin(115200);
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  WiFi.softAP(ssid, pwd);
  Udp.begin(8888);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  printToScreen("WIFI GRATIS", "IP: " + String(IP), "SSID: " + String(ssid), "PWD: " + String(pwd));
  delay(5000);
}

void loop() {
  int analogValue = analogRead(39);
  Serial.printf("Analog value: %d\n", analogValue);
  OSCBundle bndl;
  //BOSCBundle's add' returns the OSCMessage so the message's 'add' can be composed together
  bndl.add("/analog").add(analogValue);
  Udp.beginPacket(outIp, outPort);
  bndl.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  bndl.empty(); // empty the bundle to free room for a new one
  printToScreen("Analog Reading", "is", String(analogValue), "Awesome");
  delay(1000);
}