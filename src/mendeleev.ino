#include <Mendeleev.h>
#include <InternalStorage.h>

#ifndef VERSION
#define VERSION "Unknown version"
#endif

/* Counter for OTA */
int read = 0;

/* ----------------------------------------------------------------------- */
/* RS485 command callbacks                                                 */
/* ----------------------------------------------------------------------- */
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

    if (*len != 7) {
        return false;
    }

    Mendeleev.setColor(data[0], data[1], data[2], data[3], data[4]);
    Mendeleev.setUv(data[5]);
    Mendeleev.setTxt(data[6]);

    return true;
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

/* ----------------------------------------------------------------------- */
/* Input interrupt handlers                                                */
/* ----------------------------------------------------------------------- */
void input0Handler()
{
    SerialUSB.println("Input 0 interrupt handler");
}

void input1Handler()
{
    SerialUSB.println("Input 1 interrupt handler");
}

void input2Handler()
{
    SerialUSB.println("Input 2 interrupt handler");
}

void input3Handler()
{
    SerialUSB.println("Input 3 interrupt handler");
}

void proximityHandler()
{
    SerialUSB.println("Proximity interrupt handler");
}

void setup() {
#ifdef DEBUG
    /* Wait until the debug terminal is connected */
    while (!SerialUSB);
#endif
    SerialUSB.begin(115200);
    SerialUSB.println("Hello Mendeleev!");
    SerialUSB.print("Version "); SerialUSB.println(VERSION);

    /* Initialize Mendeleev board */
    Mendeleev.init();
    Mendeleev.RS485Begin(38400);

    /* Turn off TXT leds (they are default on when powering the board) */
    Mendeleev.setTxt(0);

    /* Register handler functions for RS485 commands */
    Mendeleev.registerCallback(COMMAND_SET_COLOR, &setColorCallback);
    Mendeleev.registerCallback(COMMAND_SET_MODE, &setModeCallback);
    Mendeleev.registerCallback(COMMAND_OTA, &otaCallback);
    Mendeleev.registerCallback(COMMAND_GET_VERSION, &getVersionCallback);
    Mendeleev.registerCallback(COMMAND_SET_OUTPUT, &setOutputCallback);

    /* Attach interrupt handlers to the inputs */
    Mendeleev.attachInputInterrupt(INPUT_0, input0Handler, CHANGE);
    Mendeleev.attachInputInterrupt(INPUT_1, input1Handler, CHANGE);
    Mendeleev.attachInputInterrupt(INPUT_2, input2Handler, CHANGE);
    Mendeleev.attachInputInterrupt(INPUT_3, input3Handler, CHANGE);
    Mendeleev.attachProximityInterrupt(proximityHandler, CHANGE);
}

void loop() {
    Mendeleev.tick();
}
