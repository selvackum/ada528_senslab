#include <SPI.h>
#include <LoRa.h>
#include <DHT11.h>

// Definerer LoRa-frekvens
#define LORA_FREQUENCY 868E6
#define DHTPIN 2  // DHT11 er koblet til digital pin 2
#define DEVICE_ID "2"

DHT11 dht11(DHTPIN);

int packid = 1;

void setup() {
    Serial.begin(9600);
    while (!Serial);
    
    Serial.println("LoRa-sender starter...");

    // Initialiserer LoRa-modulen
    if (!LoRa.begin(LORA_FREQUENCY)) {
        Serial.println("Feil: Kunne ikke starte LoRa-modulen!");
        while (1);
    }

    Serial.println("LoRa-sender er klar!");

    LoRa.setTxPower(30);
    //txPower - TX power in dB, defaults to 17
    //outputPin - (optional) PA output pin, supported values are PA_OUTPUT_RFO_PIN and PA_OUTPUT_PA_BOOST_PIN, defaults to PA_OUTPUT_PA_BOOST_PIN.
    //Supported values are 2 to 20 for PA_OUTPUT_PA_BOOST_PIN, and 0 to 14 for PA_OUTPUT_RFO_PIN.
    //Most modules have the PA output pin connected to PA BOOST,

    //Spreading Factor
    //Change the spreading factor of the radio.
    LoRa.setSpreadingFactor(12);
    //spreadingFactor - spreading factor, defaults to 7
    //Supported values are between 6 and 12. If a spreading factor of 6 is set, implicit header mode must be used to transmit and receive packets.


    //LoRa.setSignalBandwidth(???);
    //sadasd 


}

void loop() {
    sendLoRaMessage();
    delay(5000);  // Sender hvert 5. sekund
    packid = packid + 1;

}

// Funksjon for å lese DHT11-sensoren og sende data via LoRa
void sendLoRaMessage() {
    int temperature = 0;
    int humidity = 0;
    int result = dht11.readTemperatureHumidity(temperature, humidity);
    
    if (result != 0) {
        Serial.println("Feil ved lesing av DHT11!");
        return;
    }

    String message = "ID: " + String(DEVICE_ID) + " nr: " + String(packid);
    //String message = "PackID: " + String(packid) + ", ID: " + String(DEVICE_ID) + ", Temp: " + String(temperature) + ", Hum: " + String(humidity) + ", Msg: HelloWorld!";
    
    Serial.println("Sender melding via LoRa:");
    Serial.println(message);
    
    LoRa.beginPacket();
    LoRa.print(message);
    LoRa.endPacket();
    
    Serial.println("Melding sendt!");
}
