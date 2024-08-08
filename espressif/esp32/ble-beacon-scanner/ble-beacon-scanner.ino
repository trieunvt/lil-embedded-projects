#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEUtils.h>

/* User-defined macros */
#define SERIAL_BAUD_RATE    115200

#define SCANNER_DURATION    5       /* seconds */
#define SCANNER_INTERVAL    100     /* milliseconds */
#define SCANNER_DELAY       1000    /* milliseconds */

#define REF_MAC_ADDRESS     "ac:23:3f:a4:49:b8"
#define REF_DATA_LENGTH     15
#define REF_COMPANY_ID      0x3906
#define REF_FRAME_TYPE      0xA3

/* Global variables */
BLEScan* scanner;

class BLEBeaconScannerCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice device) {
        String mac = String(device.getAddress().toString().c_str());

        if(mac == String(REF_MAC_ADDRESS)) {
            String data = String(device.getManufacturerData().c_str());

            if(data.length() >= REF_DATA_LENGTH) {
                int companyID = (int)data.charAt(0) * 256 + (int)data.charAt(1);
                int frameType = (int)data.charAt(2);
                if((companyID == REF_COMPANY_ID) && (frameType == REF_FRAME_TYPE)) {
                    int frameVersion = (int)data.charAt(3);
                    int bateryLevel =  (int)data.charAt(4);
                    double temperatureValue = (int)data.charAt(5) * 1.0
                                            + ((int)data.charAt(6) * 1.0 / 256.0);
                    double humidityValue = (int)data.charAt(7) * 1.0
                                         + ((int)data.charAt(8) * 1.0 / 256.0);
                    char macAddress[6];
                    macAddress[0] = data.charAt(9);
                    macAddress[1] = data.charAt(10);
                    macAddress[2] = data.charAt(11);
                    macAddress[3] = data.charAt(12);
                    macAddress[4] = data.charAt(13);
                    macAddress[5] = data.charAt(14);

                    Serial.printf("[%02x:%02x:%02x:%02x:%02x:%02x] T: %5.5lf, H: %5.5lf, B: %03d\%\n",
                                    macAddress[0], macAddress[1], macAddress[2],
                                    macAddress[3], macAddress[4], macAddress[5],
                                    temperatureValue, humidityValue, bateryLevel);
                }
            }
        }
    }
};

void setup() {
    /* Set up serial debug */
    Serial.begin(SERIAL_BAUD_RATE);
    while (!Serial);
    Serial.println("\nBLE BEACON SCANNER\n");

    /* Set up BLE beacon scanner */
    BLEDevice::init("");
    scanner = BLEDevice::getScan();
    scanner->setAdvertisedDeviceCallbacks(new BLEBeaconScannerCallbacks());
    scanner->setActiveScan(true);
    scanner->setInterval(SCANNER_INTERVAL);
    scanner->setWindow(SCANNER_INTERVAL - 1);
}

void loop() {
    BLEScanResults devices = scanner->start(SCANNER_DURATION, false);

    Serial.print("The number of devices found: ");
    Serial.println(devices.getCount());
    Serial.println("Done!\n");

    scanner->clearResults();
    delay(SCANNER_DELAY);
}
