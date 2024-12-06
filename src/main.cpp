#include <Wire.h>
#include <HardwareSerial.h>

#define TLX493D_ADDRESS 0x35 // Indirizzo I2C del sensore
#define NUM_BUTTONS 4        // Numero di pulsanti sul dispositivo (provvisorio da definire come ponout)

float bx = 0.0, by = 0.0, bz = 0.0; // Variabili per i dati magnetici 
bool sendDataEnabled = false;       // Flag per abilitare l'invio dei dati
uint16_t buttonStates = 0x0000;     // Stato dei pulsanti (4 bit) - da verificare come legge questi bit (max 16) il demone spacenav

// Inizializzare il sensore magnetico
void initSensor() {
    Wire.begin();
    Wire.beginTransmission(TLX493D_ADDRESS);
    Wire.write(0x00);
    Wire.write(0x01);
    Wire.endTransmission();
}

// Aggiorna lo stato dei pulsanti (DEBUG simula i pulsanti anche se non ci sono)
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

    Wire.requestFrom(TLX493D_ADDRESS, 6); // Legge 6 byte
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

// Funzione per inviare un pacchetto di dati SpaceNav
void sendSpaceNavPacket() {
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
    memcpy(packet + 12, &buttonStates, 2); // Stato dei pulsanti(forse)

    Serial.write(packet, 14); // Invia i pacchetti binari
}

// Funzione per gestire i comandi SpaceNav (principalmente inizializzazione)
void processSpaceNavCommands() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\r'); // Legge il comando

        if (command == "@RESET") {
            // Risponde con identificazione dopo il reset
            sendDataEnabled = false;
            Serial.print("@1\r"); //la risposta deve iniziare per @1 poi ci può essere un treno di dati che identifica anche i pulsanti da capire bene
        } else if (command == "vQ") {
            // Risposta a vQ per identificazione
            Serial.print("vMagellan SpaceMouse\r");
        } else if (command == "m3") {
           // Abilita la compresisone dari ma non ho capito in che casistica si usa
        } else if (command == "c3B") {
        // Abilita modalità binaria (movimenti 3D) noi abbiamo solo quella quindi non fa un cazzo
        } else if (command == "MSSV") { //Abilita invio darti binari 
            sendDataEnabled = true;
        } else if (command == "k") { 
            //Qualcosa che ha a che fare con i tasti ma non so in che modo 
        }
    }
}

void setup() {
    Serial.begin(9600); 
    Wire.begin();       
    initSensor();       // sensore magnetico

    // Disabilita inizialmente l'invio dei dati e la modalità binaria (perchè attende il resetper l'inizializzazione)
    sendDataEnabled = false;
}

void loop() {
    processSpaceNavCommands(); // Gestisce i comandi SpaceNav
    updateButtonStates();      // Aggiorna lo stato dei pulsanti

    // Invia i pacchetti solo se abilitato
    if (sendDataEnabled) {
        readMagneticField();
        sendSpaceNavPacket();
        delay(10); // Frequenza di invio deve essere di 100 Hz
    }
}
