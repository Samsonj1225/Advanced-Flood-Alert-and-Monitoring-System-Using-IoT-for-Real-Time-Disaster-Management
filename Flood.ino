#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// === WiFi Setup ===
const char* ssid = "********";
const char* password = "********";

// === Pushover Setup ===
String pushoverUserKey = "**********************";
String pushoverAPIKey = "******************";

// === ThingSpeak Setup ===
String thingSpeakApiKey = "************";

// === MQTT Setup ===
const char* mqttServer = "test.mosquitto.org";
const int mqttPort = 1883;
const char* mqttTopic = "flood/alert/system";
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// === Pins ===
const int trigPin = 5;
const int echoPin = 18;
const int floatSensorPin = 19;
const int buzzer = 21;
const int led = 22;
const int pump = 23;

// === Threshold ===
const float waterThreshold = 50.0;

void setup() {
  Serial.begin(115200);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(floatSensorPin, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(pump, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  mqttClient.setServer(mqttServer, mqttPort);
  while (!mqttClient.connected()) mqttClient.connect("ESP32Client");

  Serial.println("Setup Complete");
}

void loop() {
  float level = getWaterLevel();
  bool overflow = digitalRead(floatSensorPin) == LOW;
  String location = getLocationByIP();

  // === Serial Monitor Output ===
  Serial.println("Water Level: " + String(level, 2) + " cm");
  Serial.println("Overflow: " + String(overflow ? "YES" : "NO"));
  Serial.println("Location: " + location);
  Serial.println("--------------------------------------------");

  logToThingSpeak(level, overflow, location);
  sendMQTT(level, overflow, location);

  // === ALERT CONDITION ===
  if (level < waterThreshold || overflow) {
    alert(location, level, overflow);
  } else {
    digitalWrite(buzzer, LOW);
    digitalWrite(led, LOW);
    digitalWrite(pump, LOW);
  }

  delay(15000);  // 15 seconds delay
}

// === Water Level Measurement ===
float getWaterLevel() {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  return (duration * 0.0343) / 2;
}

// === Get Location using ipinfo.io ===
String getLocationByIP() {
  HTTPClient http;
  http.begin("http://ipinfo.io/json");
  int httpCode = http.GET();
  String location = "Unknown Location";

  if (httpCode == 200) {
    String payload = http.getString();
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);
    String city = doc["city"];
    String region = doc["region"];
    String country = doc["country"];
    location = city + ", " + region + ", " + country;
  }

  http.end();
  return location;
}

// === Log to ThingSpeak ===
void logToThingSpeak(float level, bool overflow, String location) {
  HTTPClient http;
  String url = "http://api.thingspeak.com/update?api_key=" + thingSpeakApiKey +
               "&field1=" + String(level) +
               "&field2=" + String(overflow) +
               "&field3=" + location;
  http.begin(url);
  http.GET();
  http.end();
}

// === MQTT Publish ===
void sendMQTT(float level, bool overflow, String location) {
  if (!mqttClient.connected()) mqttClient.connect("ESP32Client");

  String message = "Flood Alert System\nWater Level: " + String(level, 2) +
                   " cm\nOverflow: " + (overflow ? "YES" : "NO") +
                   "\nLocation: " + location;

  mqttClient.publish(mqttTopic, message.c_str());
}

// === Send Alert ===
void alert(String location, float level, bool overflow) {
  digitalWrite(buzzer, HIGH);
  digitalWrite(led, HIGH);
  digitalWrite(pump, HIGH);
  delay(3000);  // Buzzer ON for 3 seconds
  digitalWrite(buzzer, LOW); // Turn off buzzer

  HTTPClient http;
  String msg = "🚨 Flood Alert System\nWater Level: " + String(level, 2) +
               " cm\nOverflow: " + (overflow ? "YES" : "NO") +
               "\nLocation: " + location;

  String url = "https://api.pushover.net/1/messages.json";
  String postData = "token=" + pushoverAPIKey +
                    "&user=" + pushoverUserKey +
                    "&message=" + msg;

  http.begin(url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.POST(postData);
  http.end();
}
