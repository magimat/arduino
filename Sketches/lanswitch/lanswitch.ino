#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

const int SWITCH_PIN = 16; 
const int POT_PIN = A0;

int curSwitchState = 1234; // nb arbitraire pour être différent de HIGH ou LOW lors de la première loop
int curPotValue = 1234;

void setup() {

  pinMode(SWITCH_PIN, INPUT);
 
  Serial.begin(9600); 
  WiFi.begin("******", "******");   
  Serial.print("connecting to wifi ");
 
  while (WiFi.status() != WL_CONNECTED) {  
    delay(500);
    Serial.print(".");
  }

  Serial.println("done!");
 
}

void sendMsg(String msg) {
  
  HTTPClient http;
  http.begin("http://192.168.11.199:39500");
  http.POST(msg);
  http.end(); 
  
  Serial.println(msg);
}

 
void loop() {
  
  int switchState = digitalRead(SWITCH_PIN);

  if(switchState != curSwitchState) {
    curSwitchState = switchState;
    if (curSwitchState == HIGH) {
      sendMsg("on");
    } else {
      sendMsg("off");
    }
  }

  float potValueFloat = analogRead(A0);
  potValueFloat = potValueFloat / 1024;
  potValueFloat = potValueFloat * 100;
  potValueFloat = round(potValueFloat);
  int potValue = (int) potValueFloat;

 

  if(abs(potValue - curPotValue) > 1) {
    curPotValue = potValue;
    sendMsg(String(potValue));
  }
  delay(1000);
}
