#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCBoards.h> 

#define RELAY_PIN 15

WiFiUDP Udp;                //objecte per a connexió udp
const unsigned int inPort  = 3333;


void actOnOpenRelay(OSCMessage &msg, int addrOffset) {
  int relayIndex = msg.getInt(0);    //desempaquetem els Integers que ens venen
  Serial.print("Opening relay #");
  digitalWrite(RELAY_PIN, HIGH);
}

void actOnCloseRelay(OSCMessage &msg, int addrOffset) {
  int relayIndex = msg.getInt(0);    //desempaquetem els Integers que ens venen
  Serial.print("Closing relay #");
  digitalWrite(RELAY_PIN, LOW);
}


void OSCMsgReceive() {
  OSCBundle bundleIN;       //paquet generic de missatges OSC, pot contenir whatever
  int size = Udp.parsePacket(); //mirem si hi ha dades als paquets, si rebem algo
  if (size > 0) {   //si rebem algo...
    Serial.print("Message Received with size: ");
    Serial.println(size);
    while (size--) {
      bundleIN.fill(Udp.read()); //plenem el bundle amb el que llegim al port Udp
      //if (bundleIN.hasError())
      //Serial.println("ERROR ON PACKET");
    }
    bundleIN.route("/open", actOnOpenRelay);  //"Rutejem el rebut a l'etiqueta i la funció que cridem mes abaix.
    bundleIN.route("/close", actOnCloseRelay);
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);

  WiFi.begin("Nasker AP", "fuckyou!");
  Serial.println("Connecting to WiFi..");
	while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  } delay(50);
	Serial.println("Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  OSCMsgReceive();  
}