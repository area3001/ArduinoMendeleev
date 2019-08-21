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
bool doUpdate = false;

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
            DEBUG_PRINTLN("Set guest mode");
            break;
        case 2:
            DEBUG_PRINTLN("Set lecturer mode");
            break;
    }

    *len = 0;
    return true;
}

bool otaCallback(uint8_t *data, uint16_t *len)
{
    DEBUG_PRINTLN("OTA callback");

    if (*len < 4) {
        DEBUG_PRINTLN("Did not receive full frame header yet");
        return true;
    }

    int i = 0;
    uint16_t index = ((data[0] << 8) | data[1]);
    uint16_t remaining = ((data[2] << 8) | data[3]);
    i += 4;

    if (read != index) {
        DEBUG_PRINTLN("OTA ERROR: restart of index!");
        InternalStorage.clear();
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
            DEBUG_PRINTLN("OTA ERROR: Internal storage is full");
            InternalStorage.clear();
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
        DEBUG_PRINTLN("Scheduling the update");
        InternalStorage.close();
        doUpdate = true;
        return true;
    }
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

bool isInBert(uint8_t id) {
    switch(id) {
        /* letter 'b' */
        case ELEMENT_H:
        case ELEMENT_Li:
        case ELEMENT_Na:
        case ELEMENT_K:
        case ELEMENT_Rb:
        case ELEMENT_Cs:
        case ELEMENT_Fr:
        case ELEMENT_Ca:
        case ELEMENT_Sc:
        case ELEMENT_Ti:
        case ELEMENT_Zr:
        case ELEMENT_Hf:
        case ELEMENT_Rf:
        case ELEMENT_Lr:
        case ELEMENT_Ra:
        /* letter 'e' */
        case ELEMENT_Fe:
        case ELEMENT_Rh:
        case ELEMENT_Ir:
        case ELEMENT_Os:
        case ELEMENT_Re:
        case ELEMENT_Mn:
        case ELEMENT_Mo:
        case ELEMENT_W:
        case ELEMENT_Sg:
        case ELEMENT_Nd:
        case ELEMENT_Np:
        case ELEMENT_Pu:
        case ELEMENT_Eu:
        /* letter 'r' */
        case ELEMENT_Cu:
        case ELEMENT_Ag:
        case ELEMENT_Au:
        case ELEMENT_Rg:
        case ELEMENT_Tb:
        case ELEMENT_Bk:
        case ELEMENT_Cd:
        case ELEMENT_Ga:
        /* letter 't' */
        case ELEMENT_N:
        case ELEMENT_P:
        case ELEMENT_As:
        case ELEMENT_Sb:
        case ELEMENT_Bi:
        case ELEMENT_Mc:
        case ELEMENT_Tm:
        case ELEMENT_Md:
        case ELEMENT_No:
        case ELEMENT_Se:
        case ELEMENT_Br:
            return true;
        default:
            return false;
    }
}

void displayBert(uint8_t address) {
    if(isInBert(address)) {
        Mendeleev.setColor(255, 0, 0, 0, 0, 0, 0);
        Mendeleev.tick();
        delay(1000);
        Mendeleev.setColor(0, 255, 0, 0, 0, 0, 0);
        Mendeleev.tick();
        delay(1000);
        Mendeleev.setColor(0, 0, 255, 0, 0, 0, 0);
        Mendeleev.tick();
        delay(1000);
        Mendeleev.setColor(0, 0, 0, 0, 0, 0, 0);
        Mendeleev.tick();
    }
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

    /* Boot animation */
    displayBert(Mendeleev.getAddress());

    /* Start communication */
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
    if (doUpdate) {
        DEBUG_PRINTLN("Executing firmware update");
        InternalStorage.apply();
        while (true);
    }
}
