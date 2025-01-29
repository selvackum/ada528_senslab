#include <WiFiS3.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include <DHT11.h>
#include <RTC.h>

// WiFi-informasjon
const char WIFI_SSID[] = "IoTstream";
const char WIFI_PASSWORD[] = "ADA528IoT";

// MQTT-informasjon
const char MQTT_BROKER_ADDRESS[] = "192.168.1.103";
const int MQTT_PORT = 1883;
const char MQTT_CLIENT_ID[] = "Gruppe2";
const char PUBLISH_TOPIC[] = "Lab1/Inngang/Bakre_";

// DHT11-informasjon
#define DHTPIN 2
DHT11 dht11(DHTPIN);

// RTC-klokke
RTClock rtcClock;

// MQTT og WiFi-klient
WiFiClient network;
MQTTClient mqtt(512);

// Sensorverdier
int temperature = 0;
int humidity = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Starter opp...");

  // Koble til WiFi
  connectToWiFi();

  // Koble til MQTT
  connectToMQTT();

  // Start RTC og sjekk om den er tilkoblet
  if (!rtcClock.begin()) {
    Serial.println("RTC ikke initialisert eller ikke tilkoblet!");
    while (1);
  }
}

void loop() {
  Serial.println("\n-- Start måleøkt --");
  
  // 1. **Mål data fra sensor**
  Serial.println("Måler data fra DHT11-sensor...");
  measureSensorData();

  // 2. **Vent i 5 sekunder, skriv status til Serial Monitor**
  for (int i = 5; i > 0; i--) {
    Serial.print("Venter... ");
    Serial.print(i);
    Serial.println(" sekunder igjen");
    delay(1000);
  }

  // 3. **Publiser til MQTT**
  Serial.println("Publiserer til MQTT...");
  publishSensorData();
  
  Serial.println("-- Slutt på måleøkt --\n");
  delay(5000); // Gir en pause før neste økt starter
}

// Koble til WiFi
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

// Koble til MQTT
void connectToMQTT() {
  Serial.println("Kobler til MQTT...");
  mqtt.begin(MQTT_BROKER_ADDRESS, MQTT_PORT, network);

  while (!mqtt.connect(MQTT_CLIENT_ID)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nMQTT tilkoblet!");
}

// Mål temperatur og fuktighet fra DHT11
void measureSensorData() {
  int result = dht11.readTemperatureHumidity(temperature, humidity);

  if (result == 0) {
    Serial.print("Temperatur: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Luftfuktighet: ");
    Serial.print(humidity);
    Serial.println(" %");
  } else {
    Serial.print("Feil under sensorlesing: ");
    Serial.println(DHT11::getErrorString(result));
  }
}

// Hent formatert tid fra RTC
String getFormattedTime() {
  RTCTime now;
  if (rtcClock.getTime(now)) {
    char timeBuffer[20];
    snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d:%02d %02d-%02d-%04d",
             now.getHour(), now.getMinutes(), now.getSeconds(),
             now.getDayOfMonth(), Month2int(now.getMonth()), now.getYear());
    return String(timeBuffer);
  } else {
    return "RTC feil";
  }
}

// Publiser data til MQTT
void publishSensorData() {
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["time"] = getFormattedTime();
  jsonDoc["temperature"] = temperature;
  jsonDoc["humidity"] = humidity;

  char jsonBuffer[512];
  serializeJson(jsonDoc, jsonBuffer);

  if (mqtt.publish(PUBLISH_TOPIC, jsonBuffer)) {
    Serial.println("Publisering vellykket!");
  } else {
    Serial.println("Feil under publisering til MQTT!");
  }
}
