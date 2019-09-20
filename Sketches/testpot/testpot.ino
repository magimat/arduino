int switchPin = 16; 
int ledPin = LED_BUILTIN; 
int val = 0;       

void setup() {
  pinMode(ledPin, OUTPUT);  // declare the ledPin as an OUTPUT
  pinMode(ledPin, OUTPUT);  // declare the ledPin as an OUTPUT
  Serial.begin(115200);
}

void loop() {
  val = analogRead(A0);    // read the value from the sensor
  
  Serial.println(val);
  digitalWrite(ledPin, HIGH);  // turn the ledPin on
  delay(val);                  // stop the program for some time
  digitalWrite(ledPin, LOW);   // turn the ledPin off
  delay(val);                  // stop the program for some time
}

// 30   = 100%
// 1024 =   0%
