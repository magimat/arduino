#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <AccelStepper.h>
#include <Timer.h>

#define HALFSTEP 8

// Motor pin definitions
#define motorPin1  5     // IN1 on the ULN2003 driver 1
#define motorPin2  4     // IN2 on the ULN2003 driver 1
#define motorPin3  0     // IN3 on the ULN2003 driver 1
#define motorPin4  2     // IN4 on the ULN2003 driver 1

// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48
//AccelStepper stepper(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);
AccelStepper stepper(AccelStepper::DRIVER, 5, 4);


const char* ssid = "*****";
const char* password = "******";

ESP8266WebServer server(80);

int curPos = 0;
int minPos = 0;
int maxPos = 105;
int inc = 25;
int motorSpeed = 30;

long timerLenght = 5000;

boolean motorOn = false;
Timer timer;



void handleMain() {

    String res = "<html><head><script>function moveTo() { var xhttp = new XMLHttpRequest(); xhttp.open('GET', 'http://store-salon.local/moveTo?pos=' + document.getElementById('pos').value, true); xhttp.send(); update();}function setSpeed() { var xhttp = new XMLHttpRequest(); xhttp.open('GET', 'http://store-salon.local/setSpeed?speed=' + document.getElementById('speed').value, true); xhttp.send(); update();}function setMaxTo() { var xhttp = new XMLHttpRequest(); xhttp.open('GET', 'http://store-salon.local/setMaxTo?pos=' + document.getElementById('max').value, true); xhttp.send(); update();}function setInc() { var xhttp = new XMLHttpRequest(); xhttp.open('GET', 'http://store-salon.local/setInc?inc=' + document.getElementById('inc').value, true); xhttp.send(); update();}function setZero() { var xhttp = new XMLHttpRequest(); xhttp.open('GET', 'http://store-salon.local/setZero', true); xhttp.send(); update();}function setMax() { var xhttp = new XMLHttpRequest(); xhttp.open('GET', 'http://store-salon.local/setMax', true); xhttp.send(); update();}function up() { var xhttp = new XMLHttpRequest(); xhttp.open('GET', 'http://store-salon.local/up', true); xhttp.send(); update();}function down() { var xhttp = new XMLHttpRequest(); xhttp.open('GET', 'http://store-salon.local/down', true); xhttp.send(); update();}function update() { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById('status').innerHTML = this.responseText; } }; xhttp.open('GET', 'http://store-salon.local/status', true); xhttp.send();}update()</script></head><body><input onClick=up() type=button value=up><br><input onClick=down() type=button value=down><br><br><span id=status></span><input type=button onClick=update() value=update><br><br><input id=max size=5><input type=button value=setMaxTo onclick='setMaxTo()'><br><input type=button onClick=setMax() value=setMaxCur><br><input type=button onClick=setZero() value=setZero><br><br><input id=inc size=5><input type=button value=setInc onclick='setInc()'><br><br><input id=speed size=5><input type=button value=setSpeed onclick='setSpeed()'><br><br><input id=pos size=5><input type=button value=moveTo onclick='moveTo()'></body></html>";
    
    server.send(200, "text/html", res);

  
}

void handleStatus() {
    String res = "increment = " + (String) inc + "<br>\n";
    res += "speeds = " + (String) motorSpeed  + "<br>\n";
    res += "curPos = " + (String) curPos + "<br>\n";
    res += "minPos = " + (String) minPos + "<br>\n";
    res += "maxPos = " + (String) maxPos + "<br>\n";
  
    server.send(200, "text/plain", res);
}

void handleMoveTo() {
    setMotorOn();
  
    int position = server.arg(0).toInt();   
    curPos = (maxPos*position) / 100;
    stepper.moveTo(curPos);

    server.send(200, "text/plain", "moved");
}


void handleSetZero() {
  curPos = 0;
  stepper.setCurrentPosition(0);
  server.send(200, "text/plain", "setzero");
}


void handleSetMax() {
  maxPos = curPos;
  server.send(200, "text/plain", "setmax");
}


void handleSetSpeed() {
    motorSpeed = server.arg(0).toInt();  
    stepper.setMaxSpeed(motorSpeed);
    server.send(200, "text/plain", "setSpeed");
}

void handleSetMaxTo() {
    maxPos = server.arg(0).toInt();   
    server.send(200, "text/plain", "setMaxTo");
}
void handleSetInc() {
    inc = server.arg(0).toInt();   
    server.send(200, "text/plain", "setInc");
}

void handleUp() {
  setMotorOn();
  curPos = curPos + inc;
  stepper.moveTo(curPos);
  server.send(200, "text/plain", "up");
}


void handleDown() {
  setMotorOn();
  curPos = curPos - inc;
  stepper.moveTo(curPos);
  server.send(200, "text/plain", "down");
}



void setMotorOff() {
   Serial.println("motor off");
   motorOn = false;
   digitalWrite(0, HIGH);
}


void setMotorOn() {
   digitalWrite(0, LOW);
   if(!motorOn) {
      motorOn = true;
      timer.after(timerLenght, setMotorOff);
   }
}


void setup() {

  pinMode(0, OUTPUT);         
  setMotorOff();
  
  stepper.setMaxSpeed(motorSpeed);
  stepper.setAcceleration(1000.0);

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  MDNS.begin("store-salon");
  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Listening on >store-salon.local<");

  server.on("/setMax", handleSetMax);
  server.on("/setMaxTo", handleSetMaxTo);
  server.on("/moveTo", handleMoveTo);
  server.on("/up", handleUp);
  server.on("/down", handleDown);
  server.on("/status", handleStatus);
  server.on("/setInc", handleSetInc);
  server.on("/setZero", handleSetZero);
  server.on("/setSpeed", handleSetSpeed);
  
 
  server.on("/", handleMain);

  
  server.begin();
  Serial.println("HTTP server started");

}

void loop() {
  server.handleClient();
  stepper.run();
  timer.update();


}
