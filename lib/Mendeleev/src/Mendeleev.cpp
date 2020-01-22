/*
 * Mendeleev.cpp
 * Company: Area 3001
 * Web: https://area3001.com
 * Description: Library for Mendeleev board
 * Version: 1.0.0
 * Date: 10/2/2019
 * Author: Bert Outtier <outtierbert@gmail.com>
 *
 * Based on https://github.com/claudeheintz/LXSAMD21DMX.git
 */

#include "Mendeleev.h"

/* RS485 protocol defines
 *
 * packet layout:
 * -------------------------------------------------------------------------------------------------------------------------
 * | Preamble (8B) | Destination (1B) | Source (1B) | Sequence (2B) | Command (1B) | Data (2B) | Data (xB) | Checksum (2B) |
 * |               |  address         |  address    |   number      |              |   length  |           |               |
 * -------------------------------------------------------------------------------------------------------------------------
 */
#define BROADCAST_ADDR        (0xFF)
#define PACKET_PREAMBLE       (0xA5)

#define PACKET_PREAMBLE_SIZE  (8u)
#define PACKET_ADDR_SIZE      (1u)
#define PACKET_SEQNR_SIZE     (2u)
#define PACKET_CMD_SIZE       (1u)
#define PACKET_LEN_SIZE       (2u)
#define PACKET_CHKSM_SIZE     (2u)

#define PACKET_DEST_OFFSET    (PACKET_PREAMBLE_SIZE)
#define PACKET_SRC_OFFSET     (PACKET_DEST_OFFSET + PACKET_ADDR_SIZE)
#define PACKET_SEQNR_OFFSET   (PACKET_SRC_OFFSET + PACKET_ADDR_SIZE)
#define PACKET_CMD_OFFSET     (PACKET_SEQNR_OFFSET + PACKET_SEQNR_SIZE)
#define PACKET_LEN_OFFSET     (PACKET_CMD_OFFSET + PACKET_CMD_SIZE)
#define PACKET_DATA_OFFSET    (PACKET_LEN_OFFSET + PACKET_LEN_SIZE)
#define PACKET_OVERHEAD       (PACKET_PREAMBLE_SIZE + (2 * PACKET_ADDR_SIZE) + PACKET_SEQNR_SIZE + PACKET_CMD_SIZE + PACKET_LEN_SIZE + PACKET_CHKSM_SIZE)

/* RS485 Pins */
#define RS485_DIR_PIN PIN_A5  /* PB02 */
#define RS485Serial   Serial  /* Sercom5 (uses UART.RXD/PB23, UART.TXD/PB22) */
#define RS485Transmit HIGH
#define RS485Receive  LOW

/* SPI CS pins */
#define PIN_SPI_CS0 (2u)            /* PA14 */
#define PIN_SPI_CS1 (7u)            /* PA21 */

/* Motors */
#define M1CTRL0_PIN PIN_SERIAL1_RX  /* PA11 SERCOM0/PAD[3] LED */
#define M1CTRL1_PIN (9u)            /* PA07 TCC1/WO[1] DIR */
#define M1CTRL2_PIN (8u)            /* PA06 TCC1/WO[0] STEP */
#define M2CTRL0_PIN PIN_SERIAL1_TX  /* PA10 SERCOM0/PAD[2] LED */
#define M2CTRL1_PIN (3u)            /* PA09 TCC0/WO[1] DIR */
#define M2CTRL2_PIN (4u)            /* PA08 TCC0/WO[0] STEP */
/* Inputs */
#define INPUT0_PIN  PIN_A1          /* PB08 */
#define INPUT1_PIN  PIN_A2          /* PB09 */
#define INPUT2_PIN  PIN_A0          /* PA02 */
#define INPUT3_PIN  PIN_A3          /* PA04 */
/* Proximity */
#define PROX_PIN    PIN_A4          /* PA05 */
/* LED pins */
#define LED_R_PIN   (5u)            /* PA15 TC3/WO[1] */
#define LED_G_PIN   (11u)           /* PA16 TCC2/WO[0] */
#define LED_B_PIN   PIN_LED_13      /* PA17 TCC2/WO[1] */
#define LED_A_PIN   (10u)           /* PA18 TC3/WO[0] */
#define LED_W_PIN   (12u)           /* PA19 TCC0/WO[3] */
#define LED_UV_PIN  (6u)            /* PA20 TCC0/WO[6] */
#define LED_TXT_PIN PIN_ATN         /* PA13 TCC2/WO[1] */

/* Dip switch pins (connected to MCP23017) */
#define DIPSW0_PIN  (0u)
#define DIPSW1_PIN  (1u)
#define DIPSW2_PIN  (2u)
#define DIPSW3_PIN  (3u)
#define DIPSW4_PIN  (4u)
#define DIPSW5_PIN  (5u)
#define DIPSW6_PIN  (6u)
#define DIPSW7_PIN  (7u)

/* Output pins (connected to MCP23017) */
#define OUTPUT0_PIN (8u)
#define OUTPUT1_PIN (9u)
#define OUTPUT2_PIN (10u)
#define OUTPUT3_PIN (11u)

/* Motor type pins (connected to MCP23017) */
#define M1TYPE0_PIN (12u)
#define M1TYPE1_PIN (13u)
#define M2TYPE0_PIN (14u)
#define M2TYPE1_PIN (15u)

/* Animation timeout in milliseconds */
#define ANIMATION_TIMEOUT (40000)

/* Table of CRC values for high-order byte */
static const uint8_t table_crc_hi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

/* Table of CRC values for low-order byte */
static const uint8_t table_crc_lo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

MendeleevClass Mendeleev;

void MendeleevClass::init()
{

    /* Set up RS485 pins */
    pinMode(RS485_DIR_PIN, OUTPUT);
    digitalWrite(RS485_DIR_PIN, RS485Receive);

    /* Set up SPI CS pins */
    // Do not use SS as slave select as it is INPUT1_PIN in Mendeleev.
    // Use the following slave selects:
    // pinMode(PIN_SPI_CS0, OUTPUT);
    // digitalWrite(PIN_SPI_CS0, HIGH);
    // pinMode(PIN_SPI_CS1, OUTPUT);
    // digitalWrite(PIN_SPI_CS1, HIGH);
    // SPI.begin();

    /* Set up motor control pins */
    // pinPeripheral(M1CTRL0_PIN, PIO_DIGITAL);
    // pinPeripheral(M2CTRL0_PIN, PIO_DIGITAL);

    pinMode(M1CTRL0_PIN, OUTPUT); // led
    pinMode(M1CTRL1_PIN, OUTPUT); // dir
    pinMode(M1CTRL2_PIN, OUTPUT); // step

    pinMode(M2CTRL0_PIN, OUTPUT); // led
    pinMode(M2CTRL1_PIN, OUTPUT); // dir
    pinMode(M2CTRL2_PIN, OUTPUT); // step

    /* Set up Input pins */
    pinMode(INPUT0_PIN, INPUT);
    pinMode(INPUT1_PIN, INPUT);
    pinMode(INPUT2_PIN, INPUT);
    pinMode(INPUT3_PIN, INPUT);
    digitalWrite(INPUT0_PIN, HIGH);
    digitalWrite(INPUT1_PIN, HIGH);
    digitalWrite(INPUT2_PIN, HIGH);
    digitalWrite(INPUT3_PIN, HIGH);

    /* Set up Proximity pin */
    pinMode(PROX_PIN, INPUT);
    digitalWrite(PROX_PIN, HIGH);

    analogWriteResolution(12);
    // pinMode(LED_R_PIN,   OUTPUT);
    // pinMode(LED_G_PIN,   OUTPUT);
    // pinMode(LED_B_PIN,   OUTPUT);
    // pinMode(LED_A_PIN,   OUTPUT);
    // pinMode(LED_W_PIN,   OUTPUT);
    // pinMode(LED_UV_PIN,  OUTPUT);
    // pinMode(LED_TXT_PIN, OUTPUT);

    /* Set up MCP23017 */
    _mcp.begin();

    /* Setup pins of MCP23017 */
    _mcp.pinMode(DIPSW0_PIN, INPUT);
    _mcp.pinMode(DIPSW1_PIN, INPUT);
    _mcp.pinMode(DIPSW2_PIN, INPUT);
    _mcp.pinMode(DIPSW3_PIN, INPUT);
    _mcp.pinMode(DIPSW4_PIN, INPUT);
    _mcp.pinMode(DIPSW5_PIN, INPUT);
    _mcp.pinMode(DIPSW6_PIN, INPUT);
    _mcp.pinMode(DIPSW7_PIN, INPUT);

    _mcp.pullUp(DIPSW0_PIN, HIGH);
    _mcp.pullUp(DIPSW1_PIN, HIGH);
    _mcp.pullUp(DIPSW2_PIN, HIGH);
    _mcp.pullUp(DIPSW3_PIN, HIGH);
    _mcp.pullUp(DIPSW4_PIN, HIGH);
    _mcp.pullUp(DIPSW5_PIN, HIGH);
    _mcp.pullUp(DIPSW6_PIN, HIGH);
    _mcp.pullUp(DIPSW7_PIN, HIGH);

    _mcp.pinMode(OUTPUT0_PIN, OUTPUT);
    _mcp.pinMode(OUTPUT1_PIN, OUTPUT);
    _mcp.pinMode(OUTPUT2_PIN, OUTPUT);
    _mcp.pinMode(OUTPUT3_PIN, OUTPUT);

    _mcp.pinMode(M1TYPE0_PIN, INPUT);
    _mcp.pinMode(M1TYPE1_PIN, INPUT);
    _mcp.pinMode(M2TYPE0_PIN, INPUT);
    _mcp.pinMode(M2TYPE1_PIN, INPUT);

    _mcp.pullUp(M1TYPE0_PIN, HIGH);
    _mcp.pullUp(M1TYPE1_PIN, HIGH);
    _mcp.pullUp(M2TYPE0_PIN, HIGH);
    _mcp.pullUp(M2TYPE1_PIN, HIGH);

    /* Read our address */
    _addr = _getAddress();

    /* Read the config pin */
    DEBUG_PRINT("Config pin is: "); DEBUG_PRINTDEC(_getConfig()); DEBUG_PRINTLN(".");

    /* Read the types of the motor slots */
    _slot1Type = getMotorType(MOTOR_1);
    DEBUG_PRINT("Motor type slot 1: "); DEBUG_PRINTDEC(_slot1Type); DEBUG_PRINTLN(".");
    if (_slot1Type != MOTORTYPE_NONE) {
        DEBUG_PRINTLN("Creating stepper in slot 1");
        digitalWrite(M1CTRL1_PIN, _motor_direction);
    }

    _slot2Type = getMotorType(MOTOR_2);
    DEBUG_PRINT("Motor type slot 2: "); DEBUG_PRINTDEC(_slot2Type); DEBUG_PRINTLN(".");
    if (_slot2Type != MOTORTYPE_NONE) {
        DEBUG_PRINTLN("Creating stepper in slot 2");
        digitalWrite(M2CTRL1_PIN, _motor_direction);
    }

    /* Set all LEDs of at boot */
    _current_colors.red = 0;
    _current_colors.green = 0;
    _current_colors.blue = 0;
    _current_colors.alpha = 0;
    _current_colors.white = 0;
    _current_colors.uv = 0;
    _current_colors.text = 0;
    _current_colors.motor1led = 0;
    _current_colors.motor2led = 0;

    /* Set some variables for the color fading */
    _fading_max_steps = 300;
    _fading_step_time = 1;
    _fading = false;
    _last_update = millis();
    _animating = false;
}

uint8_t MendeleevClass::getAddress()
{
    return _addr;
}

/* ----------------------------------------------------------------------- */
/* RS485 methods. */
/* ----------------------------------------------------------------------- */

void MendeleevClass::RS485Begin(unsigned long baud)
{
    RS485Begin(baud, SERIAL_8N1);
}

void MendeleevClass::RS485Begin(unsigned long baud, uint16_t config)
{
    RS485Serial.begin(baud, config);
}

void MendeleevClass::RS485End()
{
    RS485Serial.end();
}

void MendeleevClass::registerCallback(enum Commands command, CommandCallback cb)
{
    _callbacks[command] = cb;
}

void MendeleevClass::unregisterCallback(enum Commands command)
{
    _callbacks[command] = NULL;
}

/**
* @brief Begin write data to RS485.
*
* @return void
*/
void RS485BeginWrite()
{
    digitalWrite(RS485_DIR_PIN, RS485Transmit);
}

/**
* @brief End write data to RS485.
*
* @return void
*/
void RS485EndWrite()
{
    RS485Serial.flush();
    digitalWrite(RS485_DIR_PIN, RS485Receive);
}

size_t MendeleevClass::RS485Write(uint8_t data)
{
    RS485BeginWrite();

    size_t result = RS485Serial.write(data);

    RS485EndWrite();

    return result;
}

size_t MendeleevClass::RS485Write(char data)
{
    return RS485Write((uint8_t)data);
}

size_t MendeleevClass::RS485Write(const char* data)
{
    RS485BeginWrite();

    size_t len = strlen(data);
    size_t result = 0;
    while (len > 0) {
        result += RS485Serial.write(*data++);
        --len;
    }

    RS485EndWrite();

    return result;
}

size_t MendeleevClass::RS485Write(uint8_t* data, uint8_t dataLen)
{
    RS485BeginWrite();

    size_t result = 0;
    for (size_t i = 0; i < dataLen; i++) {
        result += RS485Serial.write(data[i]);
    }

    RS485EndWrite();

    return result;
}

int MendeleevClass::RS485Read()
{
    return RS485Read(10, 10);
}

int MendeleevClass::RS485Read(unsigned long delayWait, uint8_t repeatTime)
{
    // If the buffer is empty, wait until the data arrives.
    while (!RS485Serial.available())
    {
        delay(delayWait);
        --repeatTime;

        if (repeatTime == 0) {
            return -1;
        }
    }

    return RS485Serial.read();
}

void MendeleevClass::tick()
{
    /*
     * RS485 tick
     */
    int i;
    static uint16_t buffPos = 0;

    /* If the buffer is empty, wait until the data arrives. */
    while (RS485Serial.available() > 0) {
        i = RS485Serial.read();
        if ((buffPos < PACKET_PREAMBLE_SIZE) && (i != PACKET_PREAMBLE)) {
            buffPos = 0;
            continue;
        }
        _dataBuffer[buffPos++] = i;
    }

    _parse(_dataBuffer, &buffPos);

    /*
     * LED tick
     */
    unsigned long current_millis = millis();
    if (_fading) {
        // Enough time since the last step ?
        if (current_millis - _last_update >= _fading_step_time) {
            _fading_step++;
            _fade();
            if (_fading_step >= _fading_max_steps) {
                _fading = false;
            }
            _last_update = current_millis;
        }
    }

    /* Set the color to the leds */
    analogWrite(LED_R_PIN,   _current_colors.red);
    analogWrite(LED_G_PIN,   _current_colors.green);
    analogWrite(LED_B_PIN,   _current_colors.blue);
    analogWrite(LED_A_PIN,   _current_colors.alpha);
    analogWrite(LED_W_PIN,   _current_colors.white);
    analogWrite(LED_UV_PIN,  _current_colors.uv);
    analogWrite(LED_TXT_PIN, _current_colors.text);
    analogWrite(M1CTRL0_PIN, _current_colors.motor1led);
    analogWrite(M2CTRL0_PIN, _current_colors.motor2led);

    /* check animation timeout */
    if ((long) (millis() - _animationStartTime) >= ANIMATION_TIMEOUT) {
        _stopAnimation();
    }
}

void MendeleevClass::startAnimation()
{
    DEBUG_PRINTLN("Starting anumation");
    if (_animating) return;

    _animationStartTime = millis();
    _animating = true;

    /* activate all outputs */
    setOutput(OUTPUT_0, 1);
    setOutput(OUTPUT_1, 1);
    setOutput(OUTPUT_2, 1);
    setOutput(OUTPUT_3, 1);

    DEBUG_PRINTLN("Outputs set");

    /* activate white, UV, text and motor leds */
    fadeColor(0, 0, 0, 0, 255, 255, 255);
    fadeMotorLed(MOTOR_1, 255);
    fadeMotorLed(MOTOR_2, 255);

    DEBUG_PRINTLN("Leds faded");
    /* activate motors */
    tone(M1CTRL2_PIN, 10000);
    tone(M2CTRL2_PIN, 10000);

    DEBUG_PRINTLN("Motors activated");
}

/* ----------------------------------------------------------------------- */
/* LED methods. */
/* ----------------------------------------------------------------------- */
void MendeleevClass::setColor(uint8_t red, uint8_t green, uint8_t blue)
{
    _current_colors.red = map(red,   0, 255, 0, 2047);
    _current_colors.green = map(green, 0, 255, 0, 2047);
    _current_colors.blue = map(blue,  0, 255, 0, 2047);
    _fading = false;
}

void MendeleevClass::fadeColor(uint8_t red, uint8_t green, uint8_t blue)
{
    _initial_colors.red = _current_colors.red;
    _initial_colors.green = _current_colors.green;
    _initial_colors.blue = _current_colors.blue;
    _target_colors.red = map(red,   0, 255, 0, 2047);
    _target_colors.green = map(green, 0, 255, 0, 2047);
    _target_colors.blue = map(blue,  0, 255, 0, 2047);
    _fading = true;
    _fading_step = 0;
}

void MendeleevClass::setColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    setColor(red, green, blue);
    _current_colors.alpha = map(alpha, 0, 255, 0, 2047);
}

void MendeleevClass::fadeColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    fadeColor(red, green, blue);
    _initial_colors.alpha = _current_colors.alpha;
    _target_colors.alpha = map(alpha, 0, 255, 0, 2047);
}

void MendeleevClass::setColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, uint8_t white)
{
    setColor(red, green, blue, alpha);
    _current_colors.white = map(white, 0, 255, 0, 4095);
}

void MendeleevClass::fadeColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, uint8_t white)
{
    fadeColor(red, green, blue, alpha);
    _initial_colors.white = _current_colors.white;
    _target_colors.white = map(white, 0, 255, 0, 4095);
}

void MendeleevClass::setColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, uint8_t white, uint8_t uv, uint8_t txt)
{
    setColor(red, green, blue, alpha, white);
    setUv(uv);
    setTxt(txt);
}

void MendeleevClass::fadeColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, uint8_t white, uint8_t uv, uint8_t txt)
{
    fadeColor(red, green, blue, alpha, white);
    fadeUv(uv);
    fadeTxt(txt);
}

void MendeleevClass::setUv(uint8_t value)
{
    _current_colors.uv = map(value, 0, 255, 0, 2047);
    _fading = false;
}

void MendeleevClass::fadeUv(uint8_t value)
{
    _initial_colors.uv = _current_colors.uv;
    _target_colors.uv = map(value, 0, 255, 0, 2047);
    _fading = true;
    _fading_step = 0;
}

void MendeleevClass::setTxt(uint8_t value)
{
    _current_colors.text = map(value, 0, 255, 0, 2047);
    _fading = false;
}

void MendeleevClass::fadeTxt(uint8_t value)
{
    _initial_colors.text = _current_colors.text;
    _target_colors.text = map(value, 0, 255, 0, 2047);
    _fading = true;
    _fading_step = 0;
}

void MendeleevClass::setMotorLed(enum Motors motor, uint8_t value)
{
    switch(motor) {
        case MOTOR_1: {
            _current_colors.motor1led = map(value, 0, 255, 0, 2047);
            break;
        }
        case MOTOR_2: {
            _current_colors.motor2led = map(value, 0, 255, 0, 2047);
            break;
        }
        default:
        DEBUG_PRINTLN("Unknown motor to set Led");
    }
    _fading = false;
}

void MendeleevClass::fadeMotorLed(enum Motors motor, uint8_t value)
{
    switch(motor) {
        case MOTOR_1: {
            _initial_colors.motor1led = _current_colors.motor1led;
            _target_colors.motor1led = map(value, 0, 255, 0, 2047);
            break;
        }
        case MOTOR_2: {
            _initial_colors.motor2led = _current_colors.motor2led;
            _target_colors.motor2led = map(value, 0, 255, 0, 2047);
            break;
        }
        default:
        DEBUG_PRINTLN("Unknown motor to fade Led");
    }
    _fading = true;
    _fading_step = 0;
}

// void MendeleevClass::setFadingSpeed(uint16_t speedTime)
// {
//     _fading_step_time = speedTime;
// }

/* ----------------------------------------------------------------------- */
/* Motor methods.                                                          */
/* ----------------------------------------------------------------------- */

enum MotorType MendeleevClass::getMotorType(enum Motors motor)
{
    uint8_t type = 0;
    uint8_t value = 0;
    switch(motor) {
        case MOTOR_1: {
            value = !_mcp.digitalRead(M1TYPE0_PIN);
            type |= value;
            value = !_mcp.digitalRead(M1TYPE1_PIN);
            type |= value << 1;
            break;
        }
        case MOTOR_2: {
            value = !_mcp.digitalRead(M2TYPE0_PIN);
            type |= value;
            value = !_mcp.digitalRead(M2TYPE1_PIN);
            type |= value << 1;
            break;
        }
        default:
        DEBUG_PRINTLN("Unknown motor slot");
        return MOTORTYPE_NONE;
    }

    return (enum MotorType)type;
}

/* ----------------------------------------------------------------------- */
/* input/Output methods. */
/* ----------------------------------------------------------------------- */

uint8_t MendeleevClass::getInput(enum Input input)
{
    switch(input) {
        case INPUT_0:
        return digitalRead(INPUT0_PIN);
        case INPUT_1:
        return digitalRead(INPUT1_PIN);
        case INPUT_2:
        return digitalRead(INPUT2_PIN);
        case INPUT_3:
        return digitalRead(INPUT3_PIN);
        default:
        DEBUG_PRINTLN("Unknown input");
        return LOW;
    }
}

void MendeleevClass::attachInputInterrupt(enum Input input, voidFuncPtr ISR, int mode)
{
    switch(input) {
        case INPUT_0:
        attachInterrupt(digitalPinToInterrupt(INPUT0_PIN), ISR, mode);
        break;
        case INPUT_1:
        attachInterrupt(digitalPinToInterrupt(INPUT1_PIN), ISR, mode);
        break;
        case INPUT_2:
        attachInterrupt(digitalPinToInterrupt(INPUT2_PIN), ISR, mode);
        break;
        case INPUT_3:
        attachInterrupt(digitalPinToInterrupt(INPUT3_PIN), ISR, mode);
        break;
        default:
        DEBUG_PRINTLN("Unknown input");
    }
}

void MendeleevClass::attachProximityInterrupt(voidFuncPtr ISR, int mode)
{
    attachInterrupt(digitalPinToInterrupt(PROX_PIN), ISR, mode);
}

void MendeleevClass::setOutput(enum Output output, uint8_t value)
{
    switch(output) {
        case OUTPUT_0:
        _mcp.digitalWrite(OUTPUT0_PIN, value);
        break;
        case OUTPUT_1:
        _mcp.digitalWrite(OUTPUT1_PIN, value);
        break;
        case OUTPUT_2:
        _mcp.digitalWrite(OUTPUT2_PIN, value);
        break;
        case OUTPUT_3:
        _mcp.digitalWrite(OUTPUT3_PIN, value);
        break;
        default:
        DEBUG_PRINTLN("Unknown output");
    }
}

/* ----------------------------------------------------------------------- */
/* Private methods. */
/* ----------------------------------------------------------------------- */
uint8_t MendeleevClass::_getAddress()
{
    uint8_t addr = 0;
    uint8_t value = 0;
    value = !_mcp.digitalRead(DIPSW0_PIN);
    addr |= value;
    value = !_mcp.digitalRead(DIPSW1_PIN);
    addr |= value << 1;
    value = !_mcp.digitalRead(DIPSW2_PIN);
    addr |= value << 2;
    value = !_mcp.digitalRead(DIPSW3_PIN);
    addr |= value << 3;
    value = !_mcp.digitalRead(DIPSW4_PIN);
    addr |= value << 4;
    value = !_mcp.digitalRead(DIPSW5_PIN);
    addr |= value << 5;
    value = !_mcp.digitalRead(DIPSW6_PIN);
    addr |= value << 6;
    return addr;
}

uint8_t MendeleevClass::_getConfig()
{
    return !_mcp.digitalRead(DIPSW7_PIN);
}

uint16_t MendeleevClass::_crc16(uint8_t *buffer, uint16_t buffer_length)
{
    uint8_t crc_hi = 0xFF; /* high CRC byte initialized */
    uint8_t crc_lo = 0xFF; /* low CRC byte initialized */
    unsigned int i; /* will index into CRC lookup */

    /* pass through message buffer */
    while (buffer_length--) {
        i = crc_hi ^ *buffer++; /* calculate the CRC  */
        crc_hi = crc_lo ^ table_crc_hi[i];
        crc_lo = table_crc_lo[i];
    }

    return (crc_hi << 8 | crc_lo);
}

void MendeleevClass::_parse(uint8_t *buf, uint16_t *len)
{
    if (*len < PACKET_OVERHEAD) {
        /* The packet is not complete yet */
        return;
    }

    /* Check if packet is for me */
    if (buf[PACKET_DEST_OFFSET] != _addr && buf[PACKET_DEST_OFFSET] != BROADCAST_ADDR) {
        DEBUG_PRINT("Destination is ");
        DEBUG_PRINTHEX(buf[PACKET_DEST_OFFSET]);
        DEBUG_PRINT(". My address is ");
        DEBUG_PRINTHEX(_addr);
        DEBUG_PRINTLN(".");
        *len = 0;
        return;
    }

    /* Check the command field */
    if (buf[PACKET_CMD_OFFSET] >= COMMAND_MAX) {
        DEBUG_PRINTLN("Invalid command");
        *len = 0;
        return;
    }

    /* Check if we have a command handler for this command */
    enum Commands cmd = (enum Commands)buf[PACKET_CMD_OFFSET];

    if (_callbacks[cmd] == NULL) {
        DEBUG_PRINTLN("No callback registered for this command");
        *len = 0;
        return;
    }

    /* Check if we have the full payload */
    uint16_t datalen = ((buf[PACKET_LEN_OFFSET] << 8) | buf[PACKET_LEN_OFFSET + 1]);

    if ((datalen + PACKET_OVERHEAD) > *len) {
        /* Not everything received yet */
        return;
    }

    /* Check the checksum */
    uint16_t cksm_received = ((buf[PACKET_DATA_OFFSET + datalen] << 8) | buf[PACKET_DATA_OFFSET + datalen + 1]);
    uint16_t cksm_calculated = _crc16(buf + PACKET_DEST_OFFSET, PACKET_DATA_OFFSET + datalen - PACKET_PREAMBLE_SIZE);

    if (cksm_received != cksm_calculated) {
        DEBUG_PRINT("ERROR CRC received ");
        DEBUG_PRINTHEX(cksm_received);
        DEBUG_PRINT(" ");
        DEBUG_PRINTHEX(cksm_calculated);
        DEBUG_PRINTLN("");
        *len = 0;
        return;
    }

    /* Run callback */
    bool cb_success = (*_callbacks[cmd])(buf + PACKET_DATA_OFFSET, &datalen);

    /* Send response if necessary */
    bool is_broadcast = (buf[PACKET_DEST_OFFSET] == BROADCAST_ADDR);

    if (!is_broadcast) {
        buf[PACKET_DEST_OFFSET] = buf[PACKET_SRC_OFFSET];
        buf[PACKET_SRC_OFFSET] = _addr;
        if (!cb_success) {
            buf[PACKET_CMD_OFFSET] = ~buf[PACKET_CMD_OFFSET];
        }
        buf[PACKET_LEN_OFFSET] = datalen >> 8;
        buf[PACKET_LEN_OFFSET + 1] = datalen & 0x00FF;
        uint16_t chksm = _crc16(buf + PACKET_DEST_OFFSET, PACKET_DATA_OFFSET + datalen - PACKET_PREAMBLE_SIZE);
        buf[PACKET_DATA_OFFSET + datalen] = chksm >> 8;
        buf[PACKET_DATA_OFFSET + datalen + 1] = chksm & 0x00FF;
        RS485Write(buf, PACKET_OVERHEAD + datalen);
    }
    *len = 0;
}

void MendeleevClass::_fade()
{
    _current_colors.red = (uint16_t)(_initial_colors.red - (_fading_step*((_initial_colors.red-(float)_target_colors.red)/_fading_max_steps)));
    _current_colors.green = (uint16_t)(_initial_colors.green - (_fading_step*((_initial_colors.green-(float)_target_colors.green)/_fading_max_steps)));
    _current_colors.blue =  (uint16_t)(_initial_colors.blue - (_fading_step*((_initial_colors.blue-(float)_target_colors.blue)/_fading_max_steps)));
    _current_colors.alpha = (uint16_t)(_initial_colors.alpha - (_fading_step*((_initial_colors.alpha-(float)_target_colors.alpha)/_fading_max_steps)));
    _current_colors.white = (uint16_t)(_initial_colors.white - (_fading_step*((_initial_colors.white-(float)_target_colors.white)/_fading_max_steps)));
    _current_colors.uv = (uint16_t)(_initial_colors.uv - (_fading_step*((_initial_colors.uv-(float)_target_colors.uv)/_fading_max_steps)));
    _current_colors.text = (uint16_t)(_initial_colors.text - (_fading_step*((_initial_colors.text-(float)_target_colors.text)/_fading_max_steps)));
    _current_colors.motor1led = (uint16_t)(_initial_colors.motor1led - (_fading_step*((_initial_colors.motor1led-(float)_target_colors.motor1led)/_fading_max_steps)));
    _current_colors.motor2led = (uint16_t)(_initial_colors.motor2led - (_fading_step*((_initial_colors.motor2led-(float)_target_colors.motor2led)/_fading_max_steps)));
}

void MendeleevClass::_stopAnimation()
{
    if (!_animating) return;
    DEBUG_PRINTLN("Stopping animation");

    /* deactivate all outputs */
    setOutput(OUTPUT_0, 0);
    setOutput(OUTPUT_1, 0);
    setOutput(OUTPUT_2, 0);
    setOutput(OUTPUT_3, 0);

    /* deactivate all leds */
    fadeColor(0, 0, 0, 0, 0, 0, 0);
    fadeMotorLed(MOTOR_1, 0);
    fadeMotorLed(MOTOR_2, 0);

    /* deactivate motors */
    noTone(M1CTRL2_PIN);
    noTone(M2CTRL2_PIN);

    /* toggle direction */
    _motor_direction = !_motor_direction;
    digitalWrite(M1CTRL1_PIN, _motor_direction);
    digitalWrite(M2CTRL1_PIN, _motor_direction);

    _animating = false;
}
