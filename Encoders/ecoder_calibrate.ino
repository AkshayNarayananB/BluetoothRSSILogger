#include <RS485.h>

uint8_t CRC_8X1[256]; // Lookup table

// Function to generate the CRC lookup table
void generateCRCTable() {
    for (uint16_t j = 0; j < 256; j++) {
        uint8_t crc = j;
        for (uint8_t i = 0; i < 8; i++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x01; // x^8 + 1 polynomial
            } else {
                crc <<= 1;
            }
        }
        CRC_8X1[j] = crc & 0xFF;
    }
}

// Function to calculate CRC using the lookup table
uint8_t calculateCRC(uint8_t *data, uint8_t length) {
    uint8_t crc = 0;
    for (uint8_t i = 0; i < length; i++) {
        crc ^= data[i];
        crc = CRC_8X1[crc]; // Use lookup table for fast processing
    }
    return crc;
}

// Function to send a command with CRC over RS485
void sendCommandWithCRC(uint8_t *command, uint8_t length) {
    uint8_t crc = calculateCRC(command, length);
    RS485.write(command, length); // Send the main command
    RS485.write(crc); // Append the CRC byte
}

void setup() {
    Serial.begin(115200);
    RS485.begin(2500000); // Set baud rate to 2.5 Mbps

    generateCRCTable(); // Generate CRC lookup table

    Serial.println("Starting eCoder Self-Calibration...");

    uint8_t calibrateCommand[] = {0x1A, 0x00, 0x00, 0x00, 0x00}; // Self-calibration command
    sendCommandWithCRC(calibrateCommand, sizeof(calibrateCommand));
}

void loop() {
    if (RS485.available()) {
        Serial.print("Response: ");
        while (RS485.available()) {
            Serial.print(RS485.read(), HEX);
            Serial.print(" ");
        }
        Serial.println();
    }
}
