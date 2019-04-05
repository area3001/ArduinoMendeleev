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

int led = 6;
bool changed = false;

/* Counter for OTA */
int read = 0;

/* ----------------------------------------------------------------------- */
/* RS485 command callbacks                                                 */
/* ----------------------------------------------------------------------- */
bool setOutputCallback(uint8_t *data, uint16_t *len)
{
    SerialUSB.println("set output callback");

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
    SerialUSB.println("set color callback");

    if (*len != 7) {
        *len = 0;
        return false;
    }

    Mendeleev.fadeColor(data[0], data[1], data[2], data[3], data[4], data[5], data[6]);

    *len = 0;
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
    led++;
    led = led % 7;
    changed = true;
}

/* ----------------------------------------------------------------------- */
/* Setup                                                                   */
/* ----------------------------------------------------------------------- */
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

    if (changed) {
        changed = false;
        switch(led) {
            case 0:
                Mendeleev.fadeColor(50, 0, 0, 0, 0, 0, 0);
                break;
            case 1:
                Mendeleev.fadeColor(0, 50, 0, 0, 0, 0, 0);
                break;
            case 2:
                Mendeleev.fadeColor(0, 0, 50, 0, 0, 0, 0);
                break;
            case 3:
                Mendeleev.fadeColor(0, 0, 0, 50, 0, 0, 0);
                break;
            case 4:
                Mendeleev.fadeColor(0, 0, 0, 0, 50, 0, 0);
                break;
            case 5:
                Mendeleev.fadeColor(0, 0, 0, 0, 0, 50, 0);
                break;
            case 6:
                Mendeleev.fadeColor(0, 0, 0, 0, 0, 0, 50);
                break;
            default:
                Mendeleev.fadeColor(0, 0, 0, 0, 0, 0, 0);
                break;
        }
    }
}
