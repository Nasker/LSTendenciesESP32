#include <Arduino.h>

#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

char ssid[] = "Nasker AP";          // your network SSID (name)
char pass[] = "fuckyou!";   

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
const unsigned int localPort = 9999;        // local port to listen for OSC packets (actually not used for sending)
OSCErrorCode error;
bool relayStatus = false;

void handleMessage1(OSCMessage &msg){
  digitalWrite(LED_BUILTIN, true);
}

void handleMessage2(OSCMessage &msg){
  digitalWrite(LED_BUILTIN, false);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.printf("Connecting to: %s\n", ssid);
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
      msg.dispatch("/1/push1", handleMessage1);
      msg.dispatch("/1/push2", handleMessage2);
    } else {
      error = msg.getError();
      Serial.print("error: ");
      Serial.println(error);
    }
  }
}