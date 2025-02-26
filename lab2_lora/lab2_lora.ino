#include <SPI.h>
#include <LoRa.h>

// Definerer LoRa-frekvens
#define LORA_FREQUENCY 868E6

void setup() {
    Serial.begin(9600);
    while (!Serial);
    
    Serial.println("LoRa-mottaker starter...");

    // Initialiserer LoRa-modulen
    if (!LoRa.begin(LORA_FREQUENCY)) {
        Serial.println("Feil: Kunne ikke starte LoRa-modulen!");
        while (1);
    }

    Serial.println("LoRa-mottaker er klar!");
}

void loop() {
    receiveLoRaMessage();
}

// Funksjon for å motta og strukturere LoRa-meldinger
void receiveLoRaMessage() {
    int packetSize = LoRa.parsePacket();
    if (packetSize > 0) {
        Serial.println("\nMottatt LoRa-pakke:");
        
        // Leser meldingen fra LoRa
        String receivedMessage = "";
        while (LoRa.available()) {
            receivedMessage += (char)LoRa.read();
        }
        
        // Henter metadata
        int rssi = LoRa.packetRssi();
        float snr = LoRa.packetSnr();
        unsigned long timestamp = millis();
        
        // Skriver data til Serial Monitor
        Serial.print("Tidsstempel: ");
        Serial.println(timestamp);
        Serial.print("Melding: ");
        Serial.println(receivedMessage);
        Serial.print("RSSI: ");
        Serial.println(rssi);
        Serial.print("SNR: ");
        Serial.println(snr);
        
        // Logger data til tekst- og CSV-fil
        logToTextFile(timestamp, receivedMessage, rssi, snr);
        logToCSVFile(timestamp, receivedMessage, rssi, snr);
    }
}

// Funksjon for å lagre Serial Monitor-data i en tekstfil
void logToTextFile(unsigned long timestamp, String message, int rssi, float snr) {
    File logFile = SD.open("SerialLog.txt", FILE_WRITE);
    if (logFile) {
        logFile.print("Tidsstempel: ");
        logFile.print(timestamp);
        logFile.print(" | Melding: ");
        logFile.print(message);
        logFile.print(" | RSSI: ");
        logFile.print(rssi);
        logFile.print(" | SNR: ");
        logFile.println(snr);
        logFile.close();
    } else {
        Serial.println("Feil: Kunne ikke åpne SerialLog.txt");
    }
}

// Funksjon for å lagre data i en CSV-fil
void logToCSVFile(unsigned long timestamp, String message, int rssi, float snr) {
    File csvFile = SD.open("SerialLog.csv", FILE_WRITE);
    if (csvFile) {
        csvFile.print(timestamp);
        csvFile.print(",");
        csvFile.print(message);
        csvFile.print(",");
        csvFile.print(rssi);
        csvFile.print(",");
        csvFile.println(snr);
        csvFile.close();
    } else {
        Serial.println("Feil: Kunne ikke åpne SerialLog.csv");
    }
}
