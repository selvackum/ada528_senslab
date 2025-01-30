# Lab i ADA528

## Lab 1: Arduino Uno R4 + DHT11 som sender måledata til en MQTT Broker og mottar meldinger som subscriber

### Introduksjon
I denne labben skal vi bruke en Arduino Uno R4 sammen med en DHT11-sensor for å måle temperatur og luftfuktighet. Dataene sendes deretter til en MQTT Broker, og vi kan også motta meldinger som en subscriber.

### Krav
- Arduino Uno R4
- DHT11-sensor
- WiFi-tilkobling
- MQTT Broker

### Oppsett
1. **Koble til DHT11-sensoren:**
   - VCC til 5V
   - GND til GND
   - Data til digital pin 2

2. **Installer nødvendige biblioteker:**
   - WiFiS3
   - MQTTClient
   - ArduinoJson
   - DHT11
   - RTC

### Kode
Koden for denne labben finner du i [ArduinoLab1.ino](lab1_mqtt/ArduinoLab1.ino) og [maalespenning.ino](lab1_mqtt/maalespenning/maalespenning.ino).

### Kompilering og opplasting
1. Åpne `ArduinoLab1.ino` i Arduino IDE.
2. Velg riktig kort og port fra `Verktøy`-menyen.
3. Klikk på `Verifiser` for å kompilere koden.
4. Klikk på `Last opp` for å laste opp koden til Arduinoen.

### Bruk
Når koden er lastet opp, vil Arduinoen:
1. Koble til WiFi-nettverket.
2. Koble til MQTT Broker.
3. Måle temperatur og luftfuktighet fra DHT11-sensoren.
4. Publisere dataene til MQTT Broker.
5. Motta meldinger fra MQTT Broker som en subscriber.

### Feilsøking
- **WiFi-tilkobling feiler:** Sjekk at SSID og passord er riktig.
- **MQTT-tilkobling feiler:** Sjekk at MQTT Broker-adressen og porten er riktig.
- **Sensorlesing feiler:** Sjekk tilkoblingene til DHT11-sensoren.

### Lisens
Dette prosjektet er lisensiert under MIT-lisensen. Se [LICENSE](LICENSE) for mer informasjon.