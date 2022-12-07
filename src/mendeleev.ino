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
#include <MendeleevOta.h>

#ifndef VERSION
#define VERSION "Unknown version"
#endif

bool doReboot = false;                 /* flag to trigger a reboot */
bool ota_in_progress = false;          /* flag which indicates if an ota is in progress */
bool setup_in_progress = false;        /* flag which indicates if a setup procedure is in progress */
bool selected_for_setup = false;       /* flag which indicates if a new address is for me */

/* ----------------------------------------------------------------------- */
/* RS485 command callbacks                                                 */
/* ----------------------------------------------------------------------- */
bool setOutputCallback(uint8_t *data, uint16_t *len)
{
    if (ota_in_progress || setup_in_progress) {
        *len = 0;
        return false;
    }

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
    if (ota_in_progress || setup_in_progress) {
        *len = 0;
        return false;
    }

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
    if (ota_in_progress || setup_in_progress) {
        *len = 0;
        return false;
    }

    DEBUG_PRINTLN("set mode callback");
    bool result = true;

    /* we expect a 1 byte payload */
    if (*len != 1) {
        *len = 0;
        return false;
    }

    enum Mode mode = (enum Mode)data[0];

    switch(mode) {
    case MODE_GUEST:
        DEBUG_PRINTLN("Set lecturer mode OFF");
        Mendeleev.setMode(mode);
        break;
    case MODE_LECTURER:
        DEBUG_PRINTLN("Set lecturer mode ON");
        Mendeleev.setMode(mode);
        break;
    default:
        DEBUG_PRINTLN("unknown mode!");
        result = false;
        break;
    }

    *len = 0;
    return result;
}

bool otaCallback(uint8_t *data, uint16_t *len)
{
    DEBUG_PRINTLN("OTA callback");
    /* We expect at least 1 byte for the index */
    if (*len < 2) {
        DEBUG_PRINTLN("OTA: Did not receive valid message!");
        *len = 0;
        return false;
    }

    uint8_t idx = data[0];

    enum OtaError err = MendeleevOta.write(idx, &data[1], (*len) - 1);

    *len = 0;
    return (err == ERROR_NO_ERROR);
}

bool getVersionCallback(uint8_t *data, uint16_t *len)
{
    if (ota_in_progress || setup_in_progress) {
        *len = 0;
        return false;
    }

    DEBUG_PRINT("Version callback: "); DEBUG_PRINTLN(VERSION);
    if (sizeof(VERSION) > (BUFF_MAX-PACKET_OVERHEAD)) {
        *len = 0;
        return false;
    }

    *len = sizeof(VERSION);
    strncpy((char *)data, VERSION, sizeof(VERSION));
    return true;
}

bool rebootCallback(uint8_t *data, uint16_t *len)
{
    if (ota_in_progress || setup_in_progress) {
        *len = 0;
        return false;
    }

    DEBUG_PRINTLN("reboot callback");
    /* set the flag to trigger a reboot */
    doReboot = true;
    *len = 0;
    return true;
}

bool setupCallback(uint8_t *data, uint16_t *len)
{
    bool result = false;
    enum Setup setup;

    if (ota_in_progress) {
        goto end;
    }

    DEBUG_PRINTLN("setup callback");
    /* we expect a 1 or 2 byte payload */
    if (*len < 1) {
        goto end;
    }

    setup = (enum Setup)data[0];
    switch(setup) {
    case SETUP_START:
        DEBUG_PRINTLN("Start setup");
        setup_in_progress = true;
        selected_for_setup = false;
        Mendeleev.setColor(0, 0, 255, 0, 0, 0, 0);
        Mendeleev.setMode(MODE_SETUP);
        break;
    case SETUP_ADDRESS:
        if (setup_in_progress && selected_for_setup && (*len == 2)) {
            uint8_t addr = data[1];
            Mendeleev.setAddress(addr);
            DEBUG_PRINT("New address set: "); DEBUG_PRINTLN(addr);
            Mendeleev.setColor(0, 255, 0, 0, 0, 0, 0);
        } else {
            goto end;
        }
        break;
    case SETUP_STOP:
        DEBUG_PRINTLN("Stop setup");
        setup_in_progress = false;
        selected_for_setup = false;
        Mendeleev.setColor(0, 0, 0, 0, 0, 0, 0);
        Mendeleev.setMode(MODE_GUEST);
        break;
    default:
        DEBUG_PRINT("Unknown setup command: "); DEBUG_PRINTLN(setup);
        goto end;
    }
    result = true;

end:
    *len = 0;
    /*
     * An acknowledge will not be sent because
     * these are broadcast messages.
     */
    return result;
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
    if (setup_in_progress) {
        /* We use the proximity sensor to indicate that
         * this element is selected to receive and save
         * the new address that will be sent by the master.
         * Also, we notify the master that we are ready
         * to receive the broadcast message containing
         * the new address.
         */
        selected_for_setup = true;
        uint8_t payload[1] = { SETUP_READY };
        Mendeleev.broadcastMessage(COMMAND_SETUP, payload, 1);
        DEBUG_PRINTLN("SETUP_READY sent");
        Mendeleev.setColor(255, 0, 255, 0, 0, 0, 0);
    } else {
        Mendeleev.startAnimation();
    }
}

#ifdef DEBUG
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
#endif

/* ----------------------------------------------------------------------- */
/* Setup                                                                   */
/* ----------------------------------------------------------------------- */
void setup() {
#ifdef DEBUG
    SerialUSB.begin(115200);
#endif
    DEBUG_PRINTLN("Hello Mendeleev!");
    DEBUG_PRINT("Version: "); DEBUG_PRINTLN(VERSION);

    /* Initialize Mendeleev board */
    Mendeleev.init();
    MendeleevOta.init();

#ifdef DEBUG
    /* get node address */
    uint8_t address = Mendeleev.getAddress();
    DEBUG_PRINT("Address: "); DEBUG_PRINTDEC(address); DEBUG_PRINTLN(".");

    /* Boot animation */
    displayName(&isInBert, address);
    delay(1000);
    displayName(&isInWim, address);
    delay(1000);
    displayName(&isInJoop, address);
#endif

    /* Start communication */
    Mendeleev.RS485Begin(38400);

    /* Register handler functions for RS485 commands */
    Mendeleev.registerCallback(COMMAND_SET_COLOR,   &setColorCallback);
    Mendeleev.registerCallback(COMMAND_SET_MODE,    &setModeCallback);
    Mendeleev.registerCallback(COMMAND_OTA,         &otaCallback);
    Mendeleev.registerCallback(COMMAND_GET_VERSION, &getVersionCallback);
    Mendeleev.registerCallback(COMMAND_SET_OUTPUT,  &setOutputCallback);
    Mendeleev.registerCallback(COMMAND_REBOOT,      &rebootCallback);
    Mendeleev.registerCallback(COMMAND_SETUP,       &setupCallback);

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

    /* tick the Mendeleev OTA lib */
    bool prev_state = ota_in_progress;
    ota_in_progress = MendeleevOta.tick();
    if (!prev_state && ota_in_progress) {
        Mendeleev.setMode(MODE_OTA);
        /* turn on blue leds when OTA in progress */
        Mendeleev.setColor(0, 0, 255, 0, 0, 0, 0);
    }
    else if (prev_state && !ota_in_progress) {
        if (MendeleevOta.state() == STATE_READY) {
            /* turn on green led when OTA was successful */
            Mendeleev.setColor(0, 255, 0, 0, 0, 0, 0);
            Mendeleev.tick();
            delay(2000);
            MendeleevOta.apply();
        }
        else if (MendeleevOta.state() == STATE_IDLE) {
            /* turn on red led when OTA failed */
            Mendeleev.setColor(255, 0, 0, 0, 0, 0, 0);
            Mendeleev.setMode(MODE_GUEST);
        }
    }

    /* tick the Mendeleev lib */
    Mendeleev.tick();

    /* check the reboot flag */
    if (doReboot) {
        DEBUG_PRINTLN("Rebooting...");
        Mendeleev.setColor(255, 0, 255, 0, 0, 0, 0);
        Mendeleev.tick();
        delay(2000);
        NVIC_SystemReset();
        while (true);
    }
}
