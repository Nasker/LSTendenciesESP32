#include <Arduino.h>

//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 866E6

//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

String LoRaData;

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
  //initialize Serial Monitor
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

  printToScreen("LORA RECEIVER", "TEST", "", "");

  Serial.println("LoRa Receiver Test");
  
  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    //received a packet
    Serial.print("Received packet ");

    //read packet
    while (LoRa.available()) {
      LoRaData = LoRa.readString();
      Serial.print(LoRaData);
    }

    //print RSSI of packet
    int rssi = LoRa.packetRssi();
    Serial.print(" with RSSI ");    
    Serial.println(rssi);

   // Dsiplay information
  printToScreen("Receiving packet:", "Value: "+LoRaData, "RSSI: "+String(rssi), "");
  }
}