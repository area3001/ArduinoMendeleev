/*
 * mendeleev.ino
 * Company: Area 3001
 * Web: https://area3001.com
 * Description: Main application for Mendeleev board
 * Version: 1.0.0
 * Date: 21/3/2019
 * Author: Bert Outtier <outtierbert@gmail.com>
 */

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
    DEBUG_PRINTLN("set output callback");

    if (*len != 2) {
        *len = 0;
        return false;
    }

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
    return true;
}

bool setColorCallback(uint8_t *data, uint16_t *len)
{
    DEBUG_PRINTLN("set color callback");

    if (*len > 7) {
        *len = 0;
        return false;
    }

    uint8_t colors[7];
    memset(colors, 0, 7);
    memcpy(colors, data, *len);

    Mendeleev.fadeColor(colors[0], colors[1], colors[2], colors[3], colors[4], colors[5], colors[6]);

    *len = 0;
    return true;
}

bool setModeCallback(uint8_t *data, uint16_t *len)
{
    DEBUG_PRINTLN("set mode callback");
    if (*len != 1) {
        *len = 0;
        return false;
    }

    switch(data[0]) {
        case 1:
        // set guest mode
        break;
        case 2:
        // set lecturer mode
        break;
    }

    *len = 0;
    return true;
}

bool otaCallback(uint8_t *data, uint16_t *len)
{
    DEBUG_PRINTLN("OTA callback");

    if (*len < 4) {
        DEBUG_PRINTLN("Did not receive full file length yet");
        return true;
    }

    int i = 0;
    uint16_t index = ((data[0] << 8) | data[1]);
    uint16_t remaining = ((data[2] << 8) | data[3]);
    i += 4;

    if (read != index) {
        DEBUG_PRINT("Restart of index! ");
        DEBUG_PRINTDEC(read);
        DEBUG_PRINT(" ");
        DEBUG_PRINTDEC(index);
        DEBUG_PRINTLN(".");
        InternalStorage.close();
        read = 0;
        *len = 0;
        return false;
    }

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
            DEBUG_PRINTLN("Internal storage is full");
            InternalStorage.close();
            read = 0;
            *len = 0;
            return false;
        }
    }

    *len = 0;

    if (remaining != 0) {
        DEBUG_PRINTLN("Waiting for next packet");
        return true;
    }
    else {
        DEBUG_PRINT("Applying the update. Length = ");
        DEBUG_PRINTDEC(read);
        DEBUG_PRINTLN(".");
        InternalStorage.close();
        InternalStorage.apply();
        while (true);
    }

    DEBUG_PRINTLN("We should never get here");
    read = 0;
    InternalStorage.clear();
    return false;
}

bool getVersionCallback(uint8_t *data, uint16_t *len)
{
    DEBUG_PRINTLN("get version callback");
    *len = sizeof(VERSION);
    strncpy((char *)data, VERSION, sizeof(VERSION));
    return true;
}

/* ----------------------------------------------------------------------- */
/* Input interrupt handlers                                                */
/* ----------------------------------------------------------------------- */
void input0Handler()
{
    DEBUG_PRINTLN("Input 0 interrupt handler");
}

void input1Handler()
{
    DEBUG_PRINTLN("Input 1 interrupt handler");
}

void input2Handler()
{
    DEBUG_PRINTLN("Input 2 interrupt handler");
}

void input3Handler()
{
    DEBUG_PRINTLN("Input 3 interrupt handler");
}

void proximityHandler()
{
    DEBUG_PRINTLN("Proximity interrupt handler");
    Mendeleev.startAnimation();
}

/* ----------------------------------------------------------------------- */
/* Setup                                                                   */
/* ----------------------------------------------------------------------- */
void setup() {
// #ifdef DEBUG
//     /* Wait until the debug terminal is connected */
//     while (!SerialUSB);
// #endif
    SerialUSB.begin(115200);
    DEBUG_PRINTLN("Hello Mendeleev!");
    DEBUG_PRINT("Version: "); DEBUG_PRINTLN(VERSION);

    /* Initialize Mendeleev board */
    Mendeleev.init();
    DEBUG_PRINT("Address: "); DEBUG_PRINTDEC(Mendeleev.getAddress()); DEBUG_PRINTLN(".");
    Mendeleev.RS485Begin(38400);

    /* Register handler functions for RS485 commands */
    Mendeleev.registerCallback(COMMAND_SET_COLOR,   &setColorCallback);
    Mendeleev.registerCallback(COMMAND_SET_MODE,    &setModeCallback);
    Mendeleev.registerCallback(COMMAND_OTA,         &otaCallback);
    Mendeleev.registerCallback(COMMAND_GET_VERSION, &getVersionCallback);
    Mendeleev.registerCallback(COMMAND_SET_OUTPUT,  &setOutputCallback);

    /* Attach interrupt handlers to the inputs */
    Mendeleev.attachInputInterrupt(INPUT_0, input0Handler, CHANGE);
    Mendeleev.attachInputInterrupt(INPUT_1, input1Handler, CHANGE);
    Mendeleev.attachInputInterrupt(INPUT_2, input2Handler, CHANGE);
    Mendeleev.attachInputInterrupt(INPUT_3, input3Handler, CHANGE);
    Mendeleev.attachProximityInterrupt(proximityHandler, FALLING);
}

/* ----------------------------------------------------------------------- */
/* Main loop                                                               */
/* ----------------------------------------------------------------------- */
void loop() {
    Mendeleev.tick();
}
