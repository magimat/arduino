#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>

ESP8266WebServer webserver(80);
WiFiClient espClient;
PubSubClient clientMQTT(espClient);


const char* ssid = "******";
const char* password = "******";
const char* mqtt_server_ip = "192.168.11.202";
const char* hostname = "security-maison";

const int SIREN_1 = 16;    // D0
const int CONTACT_1 = 5;   // D1
const int CONTACT_2 = 4;   // D2

const int CONTACT_3 = 10;  // SDD3
const int CONTACT_4 = 3;   // RXD0 (can't use Serial because using this pin, may receive garbage during boot)

const int CONTACT_5 = 14;  // D5
const int MOTION_1 = 12;   // D6
const int MOTION_2 = 13;   // D7

int contacts[] = {CONTACT_1, CONTACT_2, CONTACT_3, CONTACT_4, CONTACT_5};
int motions[] = {MOTION_1, MOTION_2};

const char* contactStates[] = {"closed", "open"};
const char* motionStates[] = {"inactive", "active"};

int contactsCurState[] = {9, 9, 9, 9, 9};
int motionsCurState[] = {9, 9};




void handleWebMain() {
  String res = "OK";
  webserver.send(200, "text/html", res);
}

void handleWebAlarmOn() {
  digitalWrite(SIREN_1, HIGH);
  webserver.send(200, "text/plain", "ok");
}


void handleWebAlarmOff() {
  digitalWrite(SIREN_1, LOW);
  webserver.send(200, "text/plain", "ok");
}




void handleDevice(int i, int devices[], int devicesCurState[], const char* deviceStates[], const char* deviceType ) {

  int PIN = devices[i - 1];
  int newState = digitalRead(PIN);
  int lastState = devicesCurState[i - 1];

  if (newState != lastState) {
    devicesCurState[i - 1] = newState;


    //  smartthings/  [DEVICETYPE]   i    /    [DEVICETYPE]
    //       1              2        3    4         5

    //  ex: smartthings/contact3/contact ,  smartthings/motion1/motion


    char deviceId[1];
    itoa(i, deviceId, 10);

    char topic[100];

    strcpy(topic, "smartthings/");
    strcat(topic, deviceType);
    strcat(topic, deviceId);
    strcat(topic, "/");
    strcat(topic, deviceType);

    //Serial.println(topic);
    //Serial.println(deviceStates[newState]);


    clientMQTT.publish(topic, deviceStates[newState]);
  }

}




void setup() {

  pinMode(SIREN_1, OUTPUT);

  pinMode(CONTACT_1, INPUT_PULLUP);
  pinMode(CONTACT_2, INPUT_PULLUP);
  pinMode(CONTACT_3, INPUT_PULLUP);
  pinMode(CONTACT_4, INPUT_PULLUP);
  pinMode(CONTACT_5, INPUT_PULLUP);

  pinMode(MOTION_1, INPUT_PULLUP);
  pinMode(MOTION_2, INPUT_PULLUP);

  //Serial.begin(115200);
  WiFi.begin(ssid, password);
  //Serial.println("");

  // Wait for wifi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }

  MDNS.begin(hostname);

  //Serial.println("");
  //Serial.print("Connected to ");
  //Serial.println(ssid);
  //Serial.print("IP address: ");
  //Serial.println(WiFi.localIP());
  //Serial.print("Listening on ");
  //Serial.print(hostname);
  //Serial.println(".local");

  webserver.on("/alarmOn", handleWebAlarmOn);
  webserver.on("/alarmOff", handleWebAlarmOff);
  webserver.on("/", handleWebMain);

  webserver.begin();
  //Serial.println("HTTP server started");

  clientMQTT.setServer(mqtt_server_ip, 1883);
  reconnectMQTT();


}


void reconnectMQTT() {
  // Loop until we're reconnected
  while (!clientMQTT.connected()) {
    //Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (clientMQTT.connect(clientId.c_str())) {
      //Serial.println("connected");
    } else {
      //Serial.print("failed, rc=");
      //Serial.print(clientMQTT.state());
      //Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  webserver.handleClient();

  if (!clientMQTT.connected()) {
    reconnectMQTT();
  }

  clientMQTT.loop();

  for (int i = 1; i <= 5; i++) {
    handleDevice(i, contacts, contactsCurState, contactStates, "contact");
  }
  for (int i = 1; i <= 2; i++) {
    handleDevice(i, motions, motionsCurState, motionStates, "motion");
  }

  delay(2000);

}
