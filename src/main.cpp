#include <Wire.h>
#include "TLx493D_inc.hpp"

using namespace ifx::tlx493d;

// Configurazione del sensore
const uint8_t POWER_PIN = 15; // Pin di alimentazione (VA ALIMENTATO)
TLx493D_A1B6 dut(Wire, TLx493D_IIC_ADDR_A0_e); // Configura il sensore con l'indirizzo corretto

float bx = 0.0, by = 0.0, bz = 0.0; // Variabili per i dati magnetici
bool sendDataEnabled = false;       // Flag per abilitare l'invio dei dati
bool binaryModeEnabled = false;     // Flag per abilitare la modalità binaria
uint16_t buttonStates = 0x0000;     // Stato dei pulsanti (4 bit, inizialmente disattivati ancora da definire pin)

// Inizializza il sensore magnetico
void initSensor() {
    // Configura il pin di alimentazione
    pinMode(POWER_PIN, OUTPUT);
    digitalWrite(POWER_PIN, HIGH); // Accende il sensore
    delay(250); // Attesa per la stabilizzazione del sensore

    // Inizializza il sensore tramite la libreria
    if (!dut.begin()) {
        Serial.println("Errore: Impossibile inizializzare il sensore."); //DEBUG
        while (1); // Arresta il programma
    }
    Serial.println("Sensore inizializzato correttamente."); //DEBUG
}

// Legge i dati dal sensore magnetico
void readMagneticField() {
    double x, y, z;

    if (dut.getMagneticField(&x, &y, &z)) {
        bx = x; // Assegna i valori letti alle variabili
        by = y;
        bz = z;
    } else {
        Serial.println("Errore: Lettura dei dati dal sensore fallita!"); //DEBUG
    }
}

// Invia i dati in formato binario in formato spacenav credo non funzioni
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

// Invia i dati in formato testuale, modalità provvisoria di debug
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

// Gestisce i comandi SpaceNav (CIRCA)
void processSpaceNavCommands() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\r'); // Legge il comando

        if (command == "@RESET") {
            sendDataEnabled = false;
            Serial.print("@1 SPACEMOUSE Version 0.1\r");
        } else if (command == "vQ") {
            Serial.print("vMagellan SpaceMouse\r");
        } else if (command == "m3") {
            //capire a cosa serve
        } else if (command == "k") {
            //capire a cosa serve
        } else if (command == "CB") {     //Credo sia la modalità 3d
            binaryModeEnabled = true;
        } else if (command == "t3") { // Modalità testuale (non standard)
            binaryModeEnabled = false;
        } else if (command == "MSSV") {
            sendDataEnabled = true;
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
