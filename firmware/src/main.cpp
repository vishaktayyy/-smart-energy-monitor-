### For firmware/src/main.cpp:
```cpp
/**
 * Smart Energy Monitoring System
 * Main firmware for ESP32
 * 
 * This firmware reads current and voltage sensors,
 * calculates power consumption, and sends data to
 * an MQTT server for processing and visualization.
 * 
 * Author: Vishal Patel
 * License: MIT
 */

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <EmonLib.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "config.h"

// Energy monitoring instance
EnergyMonitor emon1;
EnergyMonitor emon2;
EnergyMonitor emon3;

// LCD Display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// WiFi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);

// Variables for energy monitoring
float voltage = 0;
float current1 = 0;
float current2 = 0;
float current3 = 0;
float power1 = 0;
float power2 = 0;
float power3 = 0;
float totalPower = 0;
float energyToday = 0;
unsigned long lastSampleTime = 0;
unsigned long lastPublishTime = 0;

// Function prototypes
void setupWiFi();
void reconnectMQTT();
void publishData();
void updateDisplay();
void calibrateSensors();
void handleMQTTMessage(char* topic, byte* payload, unsigned int length);

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("Smart Energy Monitor starting...");
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.print("Energy Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  
  // Initialize energy monitoring
  emon1.current(CURRENT_SENSOR_1_PIN, CURRENT_CALIBRATION);
  emon2.current(CURRENT_SENSOR_2_PIN, CURRENT_CALIBRATION);
  emon3.current(CURRENT_SENSOR_3_PIN, CURRENT_CALIBRATION);
  emon1.voltage(VOLTAGE_SENSOR_PIN, VOLTAGE_CALIBRATION, PHASE_CALIBRATION);
  
  // Setup WiFi
  setupWiFi();
  
  // Setup MQTT
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(handleMQTTMessage);
  
  // Initialize web server for configuration
  // (Code for web server setup would go here)
  
  Serial.println("Setup complete");
  lcd.clear();
  lcd.print("System Ready");
  delay(1000);
}

void loop() {
  // Ensure WiFi and MQTT connections
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();
  
  // Read sensors every SAMPLE_INTERVAL
  unsigned long currentMillis = millis();
  if (currentMillis - lastSampleTime >= SAMPLE_INTERVAL) {
    lastSampleTime = currentMillis;
    
    // Read voltage
    emon1.calcVI(20, 2000);
    voltage = emon1.Vrms;
    
    // Read currents
    current1 = emon1.calcIrms(1480);
    current2 = emon2.calcIrms(1480);
    current3 = emon3.calcIrms(1480);
    
    // Calculate power
    power1 = voltage * current1;
    power2 = voltage * current2;
    power3 = voltage * current3;
    totalPower = power1 + power2 + power3;
    
    // Update energy consumption
    energyToday += totalPower * (SAMPLE_INTERVAL / 3600000.0); // kWh
    
    // Update display
    updateDisplay();
  }
  
  // Publish data every PUBLISH_INTERVAL
  if (currentMillis - lastPublishTime >= PUBLISH_INTERVAL) {
    lastPublishTime = currentMillis;
    publishData();
  }
  
  // Handle web server requests
  // (Code for web server handling would go here)
}

void setupWiFi() {
  lcd.clear();
  lcd.print("Connecting WiFi");
  
  Serial.println("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcd.setCursor(0, 1);
    lcd.print("Waiting...     ");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  lcd.clear();
  lcd.print("WiFi Connected");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay(2000);
}

void reconnectMQTT() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "EnergyMonitor-";
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect
    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("connected");
      
      // Subscribe to topics
      client.subscribe(MQTT_TOPIC_CONFIG);
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void publishData() {
  // Create JSON document
  StaticJsonDocument<256> doc;
  doc["device_id"] = DEVICE_ID;
  doc["voltage"] = voltage;
  doc["current1"] = current1;
  doc["current2"] = current2;
  doc["current3"] = current3;
  doc["power1"] = power1;
  doc["power2"] = power2;
  doc["power3"] = power3;
  doc["total_power"] = totalPower;
  doc["energy_today"] = energyToday;
  doc["timestamp"] = millis();
  
  // Serialize JSON to string
  char buffer[256];
  serializeJson(doc, buffer);
  
  // Publish to MQTT
  client.publish(MQTT_TOPIC_POWER, buffer);
  Serial.println("Data published to MQTT");
}

void updateDisplay() {
  lcd.clear();
  lcd.print("Power: ");
  lcd.print(totalPower);
  lcd.print(" W");
  
  lcd.setCursor(0, 1);
  lcd.print("Energy: ");
  lcd.print(energyToday);
  lcd.print(" kWh");
}

void handleMQTTMessage(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  // Convert payload to string
  char message[length + 1];
  for (unsigned int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
  Serial.println(message);
  
  // Handle configuration messages
  if (strcmp(topic, MQTT_TOPIC_CONFIG) == 0) {
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, message);
    
    if (!error) {
      // Process configuration
      if (doc.containsKey("calibrate") && doc["calibrate"].as<bool>()) {
        calibrateSensors();
      }
    }
  }
}

void calibrateSensors() {
  // Calibration procedure would go here
  Serial.println("Starting sensor calibration...");
  lcd.clear();
  lcd.print("Calibrating...");
  
  // Placeholder for actual calibration code
  delay(3000);
  
  lcd.clear();
  lcd.print("Calibration");
  lcd.setCursor(0, 1);
  lcd.print("Complete!");
  Serial.println("Calibration complete");
}
