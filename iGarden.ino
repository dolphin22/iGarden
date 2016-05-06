#include <ESP8266WiFi.h>

const char* ssid = "CafeLAB";
const char* pass = "11235813";

// Thingspeak
String address = "api.thingspeak.com";
String writeAPI = "9V4ESBPKLGPC028M";
String readAPI = "DP32GGGD1C5R0VRU";

#define SOIL_THRESHOLD 200  // below threshold is wet, above is drive
#define FREQUENCY  60000

const int relayPin = 16;
const int soilSensor = A0;
int soilMoisture;
void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.print("Connecing to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("."); 
  }

  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // put your setup code here, to run once:
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // turn off relay
}

void loop() {
  soilMoisture = analogRead(soilSensor);
  Serial.println(soilMoisture);
  // put your main code here, to run repeatedly:
  if(soilMoisture > SOIL_THRESHOLD) { // turn on pump when soil is dry
    Serial.println("Relay turns on");
    digitalWrite(relayPin, LOW);
  } else {
    Serial.println("Relay turns off");
    digitalWrite(relayPin, HIGH);
  }

  // connect to thingspeak
  Serial.print("Connecting to ");
  Serial.println(address);

  WiFiClient client;
  const int httpPort = 80;
  if(!client.connect("api.thingspeak.com", httpPort)) {
    Serial.println("connection failed");
    return;
  }

  String postData = writeAPI;
  postData += "&field1=";
  postData += !digitalRead(relayPin);
  postData += "&field3=";
  postData += String(soilMoisture);

  client.print("POST /update HTTP/1.1\n");
  client.print("Host: api.thingspeak.com\n");
  client.print("Connection: close\n");
  client.print("X-THINGSPEAKAPIKEY: " + writeAPI + "\n");
  client.print("Content-Type: application/x-www-form-urlencoded\n");
  client.print("Content-Length: ");
  client.print(postData.length());
  client.print("\n\n");
  client.print(postData);
  
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");
  
  delay(FREQUENCY);
}
