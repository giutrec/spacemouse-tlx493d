#include <Wire.h>
#include <HardwareSerial.h>

#define TLX493D_ADDRESS 0x35 // Indirizzo I2C del sensore e' ERRATO
#define NUM_BUTTONS 4        // Numero di pulsanti sul dispositivo da finire di implementare

float bx = 0.0, by = 0.0, bz = 0.0; // Variabili per i dati magnetici
bool sendDataEnabled = false;       // Flag per abilitare l'invio dei dati
bool binaryModeEnabled = false;     // Flag per abilitare la modalità binaria
uint16_t buttonStates = 0x0000;     // Stato dei pulsanti (4 bit)

// Inizializzare il sensore magnetico
void initSensor() {
    Wire.begin();
    Wire.beginTransmission(TLX493D_ADDRESS);
    Wire.write(0x00);
    Wire.write(0x01);
    Wire.endTransmission();
}

// Aggiorna lo stato dei pulsanti (DEBUG)
void updateButtonStates() {
    static bool toggle = false;
    toggle = !toggle;
    buttonStates = toggle ? 0b00001111 : 0b00000000; // Simula alternanza
}

// Legge i dati dal sensore magnetico
void readMagneticField() {
    uint8_t buffer[6];

    Wire.beginTransmission(TLX493D_ADDRESS);
    Wire.write(0x00);
    Wire.endTransmission();

    Wire.requestFrom(TLX493D_ADDRESS, 6); // Legge 6 byte perchè non ci interessa la temperatura 
    for (int i = 0; i < 6; i++) {
        if (Wire.available()) {
            buffer[i] = Wire.read();
        }
    }

    // Converti i dati grezzi in float
    bx = (float)((int16_t)((buffer[0] << 8) | buffer[1])) / 1000.0;
    by = (float)((int16_t)((buffer[2] << 8) | buffer[3])) / 1000.0;
    bz = (float)((int16_t)((buffer[4] << 8) | buffer[5])) / 1000.0;
}

// Invia i dati in formato binario
void sendBinaryPacket() {
    int16_t trans_x = (int16_t)(bx * 1000);
    int16_t trans_y = (int16_t)(by * 1000);
    int16_t trans_z = (int16_t)(bz * 1000);

    int16_t rot_x = (int16_t)(atan2(by, bz) * 1000);
    int16_t rot_y = (int16_t)(atan2(bx, bz) * 1000);
    int16_t rot_z = (int16_t)(atan2(bx, by) * 1000);

    uint8_t packet[14];
    memcpy(packet, &trans_x, 2);
    memcpy(packet + 2, &trans_y, 2);
    memcpy(packet + 4, &trans_z, 2);
    memcpy(packet + 6, &rot_x, 2);
    memcpy(packet + 8, &rot_y, 2);
    memcpy(packet + 10, &rot_z, 2);
    memcpy(packet + 12, &buttonStates, 2);

    Serial.write(packet, 14); // Invia pacchetto binario
}

// Invia i dati in formato testuale
void sendTextualPacket() {
    Serial.print("X:");
    Serial.print(bx, 3);
    Serial.print(" Y:");
    Serial.print(by, 3);
    Serial.print(" Z:");
    Serial.print(bz, 3);
    Serial.print(" Buttons:");
    Serial.println(buttonStates, BIN);
}

// Gestisce i comandi SpaceNav
void processSpaceNavCommands() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\r'); // Legge il comando

        if (command == "@RESET") {
            sendDataEnabled = false;
            Serial.print("@1 MAGELLAN Version 6.70\r"); //provvisorio per far identificare la nostra scheda al client 
        } else if (command == "vQ") {
            Serial.print("vMagellan SpaceMouse\r");
        } else if (command == "m3") {
            binaryModeEnabled = true;
//DEBUG            Serial.print("OK\r");
        } else if (command == "t3") { // comando per modalità testuale NON previsto da spacenav 
            binaryModeEnabled = false;
//DEBUG            Serial.print("OK\r");
        } else if (command == "MSSV") {
            sendDataEnabled = true;
//DEBUG            Serial.print("OK\r");
        }
    }
}

void setup() {
    Serial.begin(9600); // Configura la comunicazione seriale
    Wire.begin();       // Inizializza I2C
    initSensor();       // Configura il sensore
    sendDataEnabled = false; // Attende comando per iniziare invio dati
}

void loop() {
    processSpaceNavCommands(); // Gestisce i comandi SpaceNav
    updateButtonStates();      // Aggiorna lo stato dei pulsanti

    if (sendDataEnabled) {
        readMagneticField();

        // Invia i dati in modalità binaria o testuale
        if (binaryModeEnabled) {
            sendBinaryPacket();
        } else {
            sendTextualPacket();
        }

        delay(10); // Frequenza di invio: 100 Hz
    }
}
