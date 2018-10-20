#include <AuthClient.h>
#include <MicroGear.h>
#include <MQTTClient.h>
#include <SHA1.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>

const char* ssid     = "JayThanawut";
const char* password = "123456788";

#define APPID       "SmartPlugKKU"
#define GEARKEY     "yr6BrAc2g0z5oqj"
#define GEARSECRET  "DCe2egLI5eO6KaF3gvgP7Qi5H"
#define SCOPE       ""
#define ALIAS       "Plug123"

WiFiClient client;
AuthClient *authclient;

int relayPin1 = D1;
int relayPin2 = D2;
int relayPin3 = D3;
int volt = 220;

MicroGear microgear(client);

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
  Serial.print("Incoming message --> ");
  Serial.print(topic);
  Serial.print(" : ");
  char strState[msglen];
  for (int i = 0; i < msglen; i++) {
    strState[i] = (char)msg[i];
    Serial.print((char)msg[i]);
  }
  Serial.println();

  String stateStr = String(strState).substring(0, msglen);

  if (stateStr == "ON1") {
    digitalWrite(relayPin1, LOW);
    microgear.chat("controllerplug", "ON");
  } else if (stateStr == "OFF1") {
    digitalWrite(relayPin1, HIGH);
    microgear.chat("controllerplug", "OFF");
  } else if (stateStr == "ON2") {
    digitalWrite(relayPin2, LOW);
    microgear.chat("controllerplug", "ON");
  } else if (stateStr == "OFF2") {
    digitalWrite(relayPin2, HIGH);
    microgear.chat("controllerplug", "OFF");
  } else if (stateStr == "ON3") {
    digitalWrite(relayPin3, LOW);
    microgear.chat("controllerplug", "ON");
  } else if (stateStr == "OFF3") {
    digitalWrite(relayPin3, HIGH);
    microgear.chat("controllerplug", "OFF");
  }

}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
  microgear.setName("pieplug");
}

void setup() {
  Serial.begin(115200);

  Serial.println("Starting...");

  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(relayPin3, OUTPUT);
  digitalWrite(relayPin1, HIGH);
  digitalWrite(relayPin2, HIGH);
  digitalWrite(relayPin3, HIGH);

  microgear.on(MESSAGE, onMsghandler);
  microgear.on(CONNECTED, onConnected);

  if (WiFi.begin(ssid, password)) {

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    //uncomment the line below if you want to reset token -->
    microgear.resetToken();
    microgear.init(GEARKEY, GEARSECRET, SCOPE);
    microgear.connect(APPID);
  }
}
 float watt;

void loop() {
  float value = analogRead(A0);
  float cur=(((((value - 526) * 5) / 1023) / 0.0645));
  Serial.println((((value - 511) * 5) / 1023) / 0.0645);
  char charBuffer[24];
 sprintf(charBuffer, "%.4f", cur);
 cur = (float)atof(charBuffer);
 
  watt =volt*cur;
  delay(250);
  if (microgear.connected()) {
    microgear.loop();
    Serial.print("Sending -- >\n");
    Serial.println(cur);
    Serial.println(value);
    
    microgear.publish("/elec", cur);
    microgear.publish("/elec/v", volt);
    microgear.publish("/elec/i3", cur);
    microgear.publish("/elec/w3", watt);
    Serial.println("Current:"+String(cur));
    if(cur>=10){
       digitalWrite(relayPin3, HIGH);
      }
    

    microgear.writeFeed("PlugKKU","Current:"+String(cur));
  } else {
    Serial.println("connection lost, reconnect...");
    microgear.connect(APPID);
  }
  delay(1000);


}
