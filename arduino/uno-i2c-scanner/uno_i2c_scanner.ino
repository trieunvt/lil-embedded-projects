/**
 * @author:  trieunvt
 * @date:    27 Jul 2024
 * @version: v1.0.0
**/

#include <Wire.h>

/* User-defined macros */
#define SERIAL_BAUD_RATE    115200
#define SCAN_MAX_ADDRESS    255
#define SCAN_NEXT_DELAY     5000    // ms

void setup() {
    /* Set up serial debug */
    Serial.begin(SERIAL_BAUD_RATE);
    while (!Serial);
    Serial.println("\nI2C SCANNER\n");

    /* Set up I2C bus */
    Wire.begin();
}

void loop() {
    int dev_address, dev_return, dev_number = 0;

    Serial.println("Scanning...\n");

    /* Scan addresses from 1 to SCAN_MAX_ADDRESS */
    for (dev_address = 1; dev_address < SCAN_MAX_ADDRESS; ++dev_address) {
        Wire.beginTransmission(dev_address);
        dev_return = Wire.endTransmission();

        if (dev_return == 0) {
            Serial.print("Available I2C device at address 0x");
            if (dev_address < 16) Serial.print("0");
            Serial.print(dev_address, HEX);
            Serial.println("\n");
            ++dev_number;
        } else if (dev_return == 4) {
            Serial.print("Unknown error at address 0x");
            if (dev_address < 16) Serial.print("0");
            Serial.print(dev_address, HEX);
            Serial.println("\n");
        }
    }

    if (dev_number == 0) Serial.println("No available I2C devices found!\n");
    else Serial.println("Done!\n");

    delay(SCAN_NEXT_DELAY);
}
