#include <Mendeleev.h>
#include <InternalStorage.h>

int read = 0;

bool setColorCallback(uint8_t *data, uint8_t len)
{
    Serial.println("set color callback");
    // Parse data and set set colors
    return true; // we want to send an ACK
}

bool setModeCallback(uint8_t *data, uint8_t len)
{
    Serial.println("set mode callback");
    // Parse data and set set colors
    return true; // we want to send an ACK
}

bool otaCallback(uint8_t *data, uint8_t len)
{
    Serial.println("OTA callback");
    int i = 0;
    uint8_t remaining = data[i++];

    while (i < len) {
        if (read < InternalStorage.maxSize()) {
            InternalStorage.write((char)data[i++]);
            read++;
        }
        else {
            // internal storage is full
            InternalStorage.clear();
            return false;
        }
    }

    if (remaining > 0) {
        InternalStorage.close();
        return true;
    }
    else {
        // apply the update
        InternalStorage.apply();
        while (true);
    }

    InternalStorage.clear();
    return false;
}

void setup() {
    while (!Serial);
    Serial.begin(115200);
    // Init Mendeleev board
    Mendeleev.init();
    // Start RS485 with bound 19200 and 8N1.
    // pin 13/14 on MKR zero
    Mendeleev.RS485Begin(19200);

    Mendeleev.registerCallback(COMMAND_SET_COLOR, &setColorCallback);
    Mendeleev.registerCallback(COMMAND_SET_MODE, &setModeCallback);
    Mendeleev.registerCallback(COMMAND_OTA, &otaCallback);
}

void loop() {
    Mendeleev.tick();
}
