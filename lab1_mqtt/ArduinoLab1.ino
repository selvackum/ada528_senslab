#include <WiFiS3.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include <DHT11.h>
#include <RTC.h> // RTC-biblioteket

// WiFi-informasjon
const char WIFI_SSID[] = "IoTstream";
const char WIFI_PASSWORD[] = "ADA528IoT";

// MQTT-informasjon
const char MQTT_BROKER_ADDRESS[] = "192.168.1.103";
const int MQTT_PORT = 1883;
const char MQTT_CLIENT_ID[] = "Gruppe2";
const char PUBLISH_TOPIC[] = "Lab1/Inngang/Bakre_";
const char SUBSCRIBE_TOPIC[] = "Lab1/#"; // Abonner på alle underliggende topics i "Lab1"

// DHT11-informasjon
#define DHTPIN 2
DHT11 dht11(DHTPIN);

// RTC-klokke
RTClock rtcClock; // Opprett RTC-klokke-instansen

// MQTT og WiFi-klient
WiFiClient network;
MQTTClient mqtt(512); // Øker bufferstørrelsen til 512 bytes

// Tidsintervaller
const int MEASURE_INTERVAL = 3000; // Måling hver 3. sekund
const int PUBLISH_INTERVAL = 5000; // Publisering hver 5. sekund
unsigned long lastMeasureTime = 0;
unsigned long lastPublishTime = 0;

// Sensorverdier
int temperature = 0;
int humidity = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Starter opp...");

  initializeDHT11();
  connectToWiFi();
  connectToMQTT();
  setupRTC();
  setupMQTTSubscription();
}

void loop() {
  mqtt.loop();
  handleSensorMeasurements();
  handleMQTTPublishing();
}

// Initialiser DHT11-sensoren
void initializeDHT11() {
  Serial.println("Initialiserer DHT11-sensor...");
}

// Funksjon for å koble til WiFi
void connectToWiFi() {
  Serial.println("Kobler til WiFi...");
  while (WiFi.begin(WIFI_SSID, WIFI_PASSWORD) != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nWiFi tilkoblet!");
  Serial.print("IP-adresse: ");
  Serial.println(WiFi.localIP());
}

// Funksjon for å koble til MQTT-broker
void connectToMQTT() {
  Serial.println("Kobler til MQTT...");
  mqtt.begin(MQTT_BROKER_ADDRESS, MQTT_PORT, network);

  while (!mqtt.connect(MQTT_CLIENT_ID)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nMQTT tilkoblet!");
  Serial.print("Tilkoblet til broker: ");
  Serial.print(MQTT_BROKER_ADDRESS);
  Serial.print(":");
  Serial.println(MQTT_PORT);
}

// Funksjon for å sette opp RTC
void setupRTC() {
  if (!rtcClock.begin()) {
    Serial.println("RTC ikke initialisert eller ikke tilkoblet!");
    while (1); // Stopp hvis RTC ikke fungerer
  }
  // Still inn tid hvis nødvendig (kun første gang, kommenter ut etterpå)
    RTCTime t(29, Month::JANUARY, 2025, 8, 29, 0, DayOfWeek::TUESDAY, SaveLight::SAVING_TIME_INACTIVE);
    rtcClock.setTime(t); // Sett dato og klokkeslett manuelt
}

// Funksjon for å sette opp MQTT-abonnement
void setupMQTTSubscription() {
  mqtt.onMessage(handleMQTTMessage);
  mqtt.subscribe(SUBSCRIBE_TOPIC);
}

// Funksjon for å håndtere sensoravlesninger
void handleSensorMeasurements() {
  if (millis() - lastMeasureTime > MEASURE_INTERVAL) {
    measureSensorData();
    lastMeasureTime = millis();
  }
}

// Funksjon for å håndtere MQTT-publisering
void handleMQTTPublishing() {
  if (millis() - lastPublishTime > PUBLISH_INTERVAL) {
    if (!mqtt.connected()) {
      Serial.println("MQTT-forbindelse mistet, prøver å koble til...");
      connectToMQTT();
    }
    publishSensorData();
    lastPublishTime = millis();
  }
}

// Funksjon for å lese data fra DHT11-sensoren
void measureSensorData() {
  int result = dht11.readTemperatureHumidity(temperature, humidity);
  if (result != 0) {
    Serial.print("Feil under sensorlesing: ");
    Serial.println(DHT11::getErrorString(result));
  }
}

// Funksjon for å hente faktisk tid fra RTC som en streng
String getFormattedTime() {
  RTCTime now;
  if (rtcClock.getTime(now)) {
    char timeBuffer[20];
    snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d:%02d",
             now.getHour(), now.getMinutes(), now.getSeconds());
    return String(timeBuffer);
  } else {
    return "RTC feil";
  }
}

// Funksjon for å publisere sensordata til MQTT
void publishSensorData() {
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["time"] = getFormattedTime();
  jsonDoc["temperature"] = temperature;
  jsonDoc["humidity"] = humidity;

  char jsonBuffer[512];
  serializeJson(jsonDoc, jsonBuffer);

  if (mqtt.publish(PUBLISH_TOPIC, jsonBuffer)) {
    Serial.println("Publisering vellykket!");
    //Serial.print("Publisert topic: ");
    //Serial.println(PUBLISH_TOPIC);
    //Serial.print("Payload: ");
    Serial.println(jsonBuffer);
  } else {
    Serial.println("Feil under publisering til MQTT!");
  }
}

// Funksjon for å håndtere innkommende MQTT-meldinger
void handleMQTTMessage(String &topic, String &payload) {
  char logBuffer[512];
  snprintf(logBuffer, sizeof(logBuffer), "Topic: %s Payload: %s", 
           topic.c_str(), payload.c_str());
  Serial.println(logBuffer);
}
