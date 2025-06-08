# Advanced-Flood-Alert-and-Monitoring-System-Using-IoT-for-Real-Time-Disaster-Management

This project is a real-time flood monitoring and alert system built using an ESP32, Ultrasonic and Float Sensors, and automated response units like a pump, buzzer, and LED. It logs water levels to ThingSpeak, sends real-time alerts via MQTT and the Pushover app, and determines approximate location using an IP-based API. The system helps reduce flood damage, provides early warning, and supports emergency response by authorities and residents.


##  Steps for ThingSpeak Setup (Cloud Logging & Visualization):
1. Go to [ThingSpeak](https://thingspeak.com) and create an account.
2. Create a new channel and enable:
   - Field1: Water Level
   - Field2: Overflow
3. Note down your **Write API Key**.
4. Use this key in your ESP32 code for logging data.
5. Open your channel dashboard to view live charts and logs.

---

## Steps for MQTT Setup (Real-Time Messaging):
1. Use the **public MQTT broker**: `test.mosquitto.org`.
2. Define your topic, e.g., `flood/alert/system`.
3. ESP32 publishes alert messages to this topic.
4. Any subscribed device will receive real-time flood warnings.

---

## Steps for Pushover Notification Setup:
1. Visit [Pushover.net](https://pushover.net/) and sign up.
2. Create an application and obtain:
   - **User Key**
   - **API Token**
3. Replace placeholders in code with your Pushover credentials.
4. ESP32 will send instant push alerts to your mobile on detecting a flood.
5. Alerts include:
   - Water Level (cm)
   - Overflow Status
   - Approximate Location

---

## Steps for IP-Based Location Tracking:
1. ESP32 sends an HTTP request to: `http://ipinfo.io/json`
2. Parses JSON data to extract:
   - City
   - Region
   - Country
3. Adds this info to ThingSpeak logs and alert messages.

---

## Steps to Connect and Upload Code (Arduino IDE):
1. Open Arduino IDE.
2. Install required libraries via Library Manager:
   - WiFi.h
   - HTTPClient.h
   - PubSubClient.h
   - ArduinoJson.h
3. Select **ESP32 Dev Module** under Tools > Board.
4. Choose the correct **COM Port**.
5. Paste the source code into a new sketch.
6. Replace the placeholders:
   - `ssid` and `password` → your Wi-Fi credentials
   - `pushoverUserKey` and `pushoverAPIKey`
   - `thingSpeakApiKey`
7. Upload the code to ESP32.
8. Monitor output via Serial Monitor.

---
