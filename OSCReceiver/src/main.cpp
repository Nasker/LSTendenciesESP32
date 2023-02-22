#include <Arduino.h>

#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

char ssid[] = "lstendencieswifi";          // your network SSID (name)
char pass[] = "arduinorules";                    // your network password

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
const unsigned int localPort = 9999;        // local port to listen for OSC packets (actually not used for sending)
OSCErrorCode error;
bool relayStatus = false;

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

void handleMessage(OSCMessage &msg){
  char* receivedString;
  if(msg.isString(0)){
    msg.getString(0, receivedString);
    relayStatus = receivedString == "ON";
  } 
  printToScreen(WiFi.localIP().toString(), String(localPort), "Received OSC message", receivedString);
  digitalWrite(BUILTIN_LED, relayStatus);
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
  pinMode(BUILTIN_LED, OUTPUT);
  printToScreen(WiFi.localIP().toString(), String(localPort),"Waiting for OSC msg", "");
}

void loop(){
    OSCMessage msg;
  int size = Udp.parsePacket();

  if (size > 0) {
    while (size--) {
      msg.fill(Udp.read());
    }
    if (!msg.hasError()) {
      char address[32];
      msg.getAddress(address);
      Serial.printf("Received message %s\n", address);
      msg.dispatch("/test", handleMessage);
    } else {
      error = msg.getError();
      Serial.print("error: ");
      Serial.println(error);
    }
  }
}