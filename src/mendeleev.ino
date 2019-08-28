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

int read = 0;                          /* Counter indicating OTA progress */
bool doUpdate = false;                 /* flag to trigger an update */
bool otaInProgress = false;
bool doReboot = false;                 /* flasg to trigger a reboot */
unsigned long lastOTAmsg;              /* last time an OTA message was received and processed */
const unsigned long OTA_TIMEOUT(5000); /* Timeout of OTA */

/* ----------------------------------------------------------------------- */
/* RS485 command callbacks                                                 */
/* ----------------------------------------------------------------------- */
bool setOutputCallback(uint8_t *data, uint16_t *len)
{
    DEBUG_PRINTLN("set output callback");

    /* we expect a 2 byte payload */
    if (*len != 2) {
        *len = 0;
        return false;
    }

    /* byte 0 is the mask, byte 1 is the value */
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

    /* we expect a payload between 3 and 7 bytes */
    if (*len > 7 || *len < 3) {
        *len = 0;
        return false;
    }

    /* set default color to off */
    uint8_t colors[7];
    memset(colors, 0, 7);
    /* set the color */
    memcpy(colors, data, *len);

    /* fade the color to the new value */
    Mendeleev.fadeColor(colors[0], colors[1], colors[2], colors[3], colors[4], colors[5], colors[6]);

    *len = 0;
    return true;
}

bool setModeCallback(uint8_t *data, uint16_t *len)
{
    DEBUG_PRINTLN("set mode callback");

    /* we expect a 1 byte payload */
    if (*len != 1) {
        *len = 0;
        return false;
    }

    switch(data[0]) {
        case 0x01:
            DEBUG_PRINTLN("Set guest mode");
            break;
        case 0x02:
            DEBUG_PRINTLN("Set lecturer mode");
            break;
    }

    *len = 0;
    return true;
}

bool otaCallback(uint8_t *data, uint16_t *len)
{
    DEBUG_PRINTLN("OTA callback");

    /* We expect at least 4 bytes */
    if (*len < 4) {
        DEBUG_PRINTLN("Did not receive full frame header yet");
        return true;
    }

    int i = 0;
    uint16_t index = ((data[0] << 8) | data[1]);     // index of the current frame
    uint16_t remaining = ((data[2] << 8) | data[3]); // remaining bytes of the firmware file to be sent
    i += 4;

    /*
     * check if the frame index of
     * the new packet follows what
     * was previously received
     */
    if (read != index) {
        DEBUG_PRINTLN("OTA ERROR: restart of index!");
        InternalStorage.clear();
        InternalStorage.close();
        read = 0;
        otaInProgress = false;
        /* is it a start of a new update? */
        if (index != 0) {
            *len = 0;
            return false;
        }
    }

    /* new update trigger */
    if (read == 0) {
        InternalStorage.open();
    }

    /* read the frame data to internal storage */
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
            otaInProgress = false;
            read = 0;
            *len = 0;
            return false;
        }
    }

    *len = 0;

    /*
     * Did we receive the last frame or
     * do we expect more frames to follow?
     */
    if (remaining != 0) {
        DEBUG_PRINTLN("Waiting for next packet");
    }
    else {
        DEBUG_PRINTLN("Scheduling the update");
        InternalStorage.close();
        doUpdate = true;
    }

    otaInProgress = true;

    /*
     * update the timestamp used
     * to check the OTA timeout
     */
    lastOTAmsg = millis();
    return true;
}

bool getVersionCallback(uint8_t *data, uint16_t *len)
{
    DEBUG_PRINTLN("get version callback");
    if (sizeof(VERSION) > BUFF_MAX) {
        *len = 0;
        return false;
    }

    *len = sizeof(VERSION);
    strncpy((char *)data, VERSION, sizeof(VERSION));
    return true;
}

bool rebootCallback(uint8_t *data, uint16_t *len)
{
    DEBUG_PRINTLN("reboot callback");
    /* set the flag to trigger a reboot */
    doReboot = true;
    *len = 0;
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

bool isInWim(uint8_t id) {
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

bool isInJoop(uint8_t id) {
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

void displayName(bool (*namefunction)(uint8_t), uint8_t address) {
    if((*namefunction)(address)) {
        Mendeleev.setColor(255, 0, 0, 0, 0, 0, 0);
        Mendeleev.tick();
        delay(500);
        Mendeleev.setColor(0, 255, 0, 0, 0, 0, 0);
        Mendeleev.tick();
        delay(500);
        Mendeleev.setColor(0, 0, 255, 0, 0, 0, 0);
        Mendeleev.tick();
        delay(500);
        Mendeleev.setColor(255, 0, 0, 0, 0, 0, 0);
        Mendeleev.tick();
        delay(500);
        Mendeleev.setColor(0, 255, 0, 0, 0, 0, 0);
        Mendeleev.tick();
        delay(500);
        Mendeleev.setColor(0, 0, 255, 0, 0, 0, 0);
        Mendeleev.tick();
        delay(500);
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

    /* get node address */
    uint8_t address = Mendeleev.getAddress();
    DEBUG_PRINT("Address: "); DEBUG_PRINTDEC(address); DEBUG_PRINTLN(".");

    /* Boot animation */
    displayName(&isInBert, address);
    delay(1000);
    displayName(&isInWim, address);
    delay(1000);
    displayName(&isInJoop, address);

    /* Start communication */
    Mendeleev.RS485Begin(38400);

    /* Register handler functions for RS485 commands */
    Mendeleev.registerCallback(COMMAND_SET_COLOR,   &setColorCallback);
    Mendeleev.registerCallback(COMMAND_SET_MODE,    &setModeCallback);
    Mendeleev.registerCallback(COMMAND_OTA,         &otaCallback);
    Mendeleev.registerCallback(COMMAND_GET_VERSION, &getVersionCallback);
    Mendeleev.registerCallback(COMMAND_SET_OUTPUT,  &setOutputCallback);
    Mendeleev.registerCallback(COMMAND_REBOOT,      &rebootCallback);

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
    /* tick the mendeleev lib */
    Mendeleev.tick();

    /* check the update flag */
    if (doUpdate) {
        DEBUG_PRINTLN("Executing firmware update");
        InternalStorage.apply();
        otaInProgress = false;
        while (true);
    }
    else if (otaInProgress) {
        unsigned long ms = millis();
        if ((ms - lastOTAmsg) >= OTA_TIMEOUT) {
            DEBUG_PRINTLN("OTA ERROR: Did not receive remaining frames for 5 seconds");
            InternalStorage.clear();
            InternalStorage.close();
            otaInProgress = false;
            read = 0;
        }
    }

    /* check the reboot flag */
    if (doReboot) {
        DEBUG_PRINTLN("Rebooting...");
        NVIC_SystemReset();
        while (true);
    }
}
