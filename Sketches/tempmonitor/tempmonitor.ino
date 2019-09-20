

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>


/* DHT SENSOR SETUP */

#define DHTTYPE DHT22
#define DHTPIN  2


const char* ssid = "******";
const char* password = "******";
const char* mqtt_server = "192.168.11.202";
const char* tempTopic = "smartthings/test/temperatureMeasurement";
const char* humidityTopic = "smartthings/test/humidity";
const long refreshDelay = 30000;



DHT dht(DHTPIN, DHTTYPE); 


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}



void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  dht.begin(); 
  float temp = dht.readTemperature();
  int hum = dht.readHumidity();
  
  char tempString[10];
  dtostrf(temp, 3, 1, tempString);

  char humString[5];
  String(hum).toCharArray(humString,5);
  
  Serial.println(tempString);
  Serial.println(hum);
  Serial.println();
  client.publish(tempTopic, tempString);
  client.publish(humidityTopic, humString);

  delay(refreshDelay);
}
