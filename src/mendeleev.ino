#include <Mendeleev.h>
#include <InternalStorage.h>

#ifndef VERSION
#define VERSION "Unknown version"
#endif

int read = 0;

bool setOutputCallback(uint8_t *data, uint16_t *len)
{
    SerialUSB.println("set output callback");
    if (bitRead(data[0], 0)) {
        Mendeleev.setOutput(OUTPUT_0, bitRead(data[1], 0));
    }
    if (bitRead(data[0], 1)) {
        Mendeleev.setOutput(OUTPUT_1, bitRead(data[1], 1));
    }
    if (bitRead(data[0], 2)) {
        Mendeleev.setOutput(OUTPUT_2, bitRead(data[1], 2));
    }
    if (bitRead(data[0], 3)) {
        Mendeleev.setOutput(OUTPUT_3, bitRead(data[1], 3));
    }
    *len = 0;
}

bool setColorCallback(uint8_t *data, uint16_t *len)
{
    SerialUSB.println("set color callback");
    *len = 0;

    if (data[0] == 0xFF) {
        return true;
    }
    else {
        return false;
    }
}

bool setModeCallback(uint8_t *data, uint16_t *len)
{
    SerialUSB.println("set mode callback");
    *len = 0;

    if (data[0] == 0xFF) {
        return true;
    }
    else {
        return false;
    }
}

bool otaCallback(uint8_t *data, uint16_t *len)
{
    SerialUSB.println("OTA callback");
    int i = 0;
    uint16_t remaining = ((data[0] << 8) | data[1]);
    i += 2;

    if (read == 0) {
        InternalStorage.open();
    }

    while (i < *len) {
        if (read < InternalStorage.maxSize()) {
            InternalStorage.write((char)data[i++]);
            remaining--;
            read++;
        }
        else {
            SerialUSB.println("Internal storage is full");
            InternalStorage.close();
            read = 0;
            *len = 0;
            return false;
        }
    }

    *len = 0;

    if (remaining != 0) {
        SerialUSB.println("Waiting for next packet");
        return true;
    }
    else {
        SerialUSB.print("Applying the update. Length = ");
        SerialUSB.println(read, DEC);
        InternalStorage.close();
        InternalStorage.apply();
        while (true);
    }

    SerialUSB.println("We should never get here");
    read = 0;
    InternalStorage.clear();
    return false;
}

bool getVersionCallback(uint8_t *data, uint16_t *len)
{
    SerialUSB.println("get version callback");
    *len = sizeof(VERSION);
    strncpy((char *)data, VERSION, sizeof(VERSION));
    return true;
}

void setup() {
#ifdef DEBUG
    while (!SerialUSB);
#endif
    SerialUSB.begin(115200);
    SerialUSB.print("Version ");
    SerialUSB.println(VERSION);

    // Init Mendeleev board
    Mendeleev.init();
    Mendeleev.RS485Begin(38400);

    Mendeleev.registerCallback(COMMAND_SET_COLOR, &setColorCallback);
    Mendeleev.registerCallback(COMMAND_SET_MODE, &setModeCallback);
    Mendeleev.registerCallback(COMMAND_OTA, &otaCallback);
    Mendeleev.registerCallback(COMMAND_GET_VERSION, &getVersionCallback);
    Mendeleev.registerCallback(COMMAND_SET_OUTPUT, &setOutputCallback);

    Mendeleev.setTxt(0);
}

void loop() {
    Mendeleev.tick();
}
