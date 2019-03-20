// Mendeleev.cpp
// Company: Area 3001
// Web: https://area3001.com
// Description:
//		Library for Meneleev board
// Version: 1.0.0
// Date: 10/2/2019
// Author: Bert Outtier <outtierbert@gmail.com>

#include <SPI.h>
#include "Mendeleev.h"
#include "wiring_private.h"

#ifdef DEBUG
 #define DEBUG_PRINTLN(x)  SerialUSB.println(x)
 #define DEBUG_PRINT(x)    SerialUSB.print(x)
 #define DEBUG_PRINTDEC(x) SerialUSB.print(x, DEC)
 #define DEBUG_PRINTHEX(x) SerialUSB.print(x, HEX)
#else
 #define DEBUG_PRINTLN(x)
 #define DEBUG_PRINT(x)
 #define DEBUG_PRINTDEC(x)
 #define DEBUG_PRINTHEX(x)
#endif

// RS485 Pins
#define RS485_DIR_PIN PIN_A5  // PB02
#define RS485Serial   Serial  // Sercom5 (uses UART.RXD/PB23, UART.TXD/PB22)
#define RS485Transmit HIGH
#define RS485Receive  LOW

// SPI CS pins
#define PIN_SPI_CS0 (2u)            // PA14
#define PIN_SPI_CS1 (7u)            // PA21

// Motors
#define M1CTRL0_PIN PIN_SERIAL1_RX  // PA11 SERCOM0/PAD[3]
#define M1CTRL1_PIN (9u)            // PA07 TCC1/WO[1]
#define M1CTRL2_PIN (8u)            // PA06 TCC1/WO[0]
#define M2CTRL0_PIN PIN_SERIAL1_TX  // PA10 SERCOM0/PAD[2]
#define M2CTRL1_PIN (3u)            // PA09 TCC0/WO[1]
#define M2CTRL2_PIN (4u)            // PA08 TCC0/WO[0]

// Inputs
#define INPUT0_PIN  PIN_A1          // PB08
#define INPUT1_PIN  PIN_A2          // PB09
#define INPUT2_PIN  PIN_A0          // PA02
#define INPUT3_PIN  PIN_A3          // PA04

// Proximity
#define PROX_PIN    PIN_A4          // PA05

// LED pins
#define LED_R_PIN   (5u)            // PA15 TC3/WO[1]
#define LED_G_PIN   (11u)           // PA16 TCC2/WO[0]
#define LED_B_PIN   PIN_LED_13      // PA17 TCC2/WO[1]
#define LED_A_PIN   (10u)           // PA18 TC3/WO[0]
#define LED_W_PIN   (12u)           // PA19 TCC0/WO[3]
#define LED_UV_PIN  (6u)            // PA20 TCC0/WO[6]
#define LED_TXT_PIN PIN_ATN         // PA13 TCC2/WO[1]

// Dip switch pins (connected to MCP23017)
#define DIPSW0_PIN  (0u)
#define DIPSW1_PIN  (1u)
#define DIPSW2_PIN  (2u)
#define DIPSW3_PIN  (3u)
#define DIPSW4_PIN  (4u)
#define DIPSW5_PIN  (5u)
#define DIPSW6_PIN  (6u)
#define DIPSW7_PIN  (7u)

// Output pins (connected to MCP23017)
#define OUTPUT0_PIN (8u)
#define OUTPUT1_PIN (9u)
#define OUTPUT2_PIN (10u)
#define OUTPUT3_PIN (11u)

// Motor type pins (connected to MCP23017)
#define M1TYPE0_PIN (12u)
#define M1TYPE1_PIN (13u)
#define M2TYPE0_PIN (14u)
#define M2TYPE1_PIN (15u)

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
    // Set up RS485 pins
    pinMode(RS485_DIR_PIN, OUTPUT);
    digitalWrite(RS485_DIR_PIN, RS485Receive);

    // Set up SPI CS pins
    // Do not use SS as slave select as it is INPUT1_PIN in Mendeleev.
    // Use the following slave selects:
    pinMode(PIN_SPI_CS0, OUTPUT);
    digitalWrite(PIN_SPI_CS0, HIGH);
    pinMode(PIN_SPI_CS1, OUTPUT);
    digitalWrite(PIN_SPI_CS1, HIGH);
    SPI.begin();

    // Set up motor control pins
    pinPeripheral(M1CTRL0_PIN, PIO_DIGITAL);
    pinPeripheral(M2CTRL0_PIN, PIO_DIGITAL);

    pinMode(M1CTRL0_PIN, OUTPUT);
    pinMode(M1CTRL1_PIN, OUTPUT);
    pinMode(M1CTRL2_PIN, OUTPUT);
    pinMode(M2CTRL0_PIN, OUTPUT);
    pinMode(M2CTRL1_PIN, OUTPUT);
    pinMode(M2CTRL2_PIN, OUTPUT);

    // Set up Input pins
    pinMode(INPUT0_PIN, INPUT);
    pinMode(INPUT1_PIN, INPUT);
    pinMode(INPUT2_PIN, INPUT);
    pinMode(INPUT3_PIN, INPUT);

    // Set up Proximity pin
    pinMode(PROX_PIN, INPUT);

    // Set up LED pins
    pinMode(LED_R_PIN,   OUTPUT);
    pinMode(LED_G_PIN,   OUTPUT);
    pinMode(LED_B_PIN,   OUTPUT);
    pinMode(LED_A_PIN,   OUTPUT);
    pinMode(LED_W_PIN,   OUTPUT);
    pinMode(LED_UV_PIN,  OUTPUT);
    pinMode(LED_TXT_PIN, OUTPUT);

    // Set up MCP23017
    _mcp.begin();

    // Setup pins of MCP23017
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

    // Read our address
    _addr = _getAddress();
    DEBUG_PRINT("I am element "); DEBUG_PRINTDEC(_addr); DEBUG_PRINTLN(".");
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
        // The packet is not complete yet
        return;
    }

    if (buf[PACKET_DEST_OFFSET] != _addr && buf[PACKET_DEST_OFFSET] != BROADCAST_ADDR) {
        DEBUG_PRINT("Destination is ");
        DEBUG_PRINTHEX(buf[PACKET_DEST_OFFSET]);
        DEBUG_PRINT(". My address is ");
        DEBUG_PRINTHEX(_addr);
        DEBUG_PRINTLN(".");
        *len = 0;
        return;
    }

    bool is_broadcast = (buf[PACKET_DEST_OFFSET] == BROADCAST_ADDR);

    if (buf[PACKET_CMD_OFFSET] >= COMMAND_MAX) {
        DEBUG_PRINTLN("Invalid command");
        *len = 0;
        return;
    }

    enum Commands cmd = (enum Commands)buf[PACKET_CMD_OFFSET];

    if (_callbacks[cmd] == NULL) {
        DEBUG_PRINTLN("No callback registered for this command");
        *len = 0;
        return;
    }

    uint16_t datalen = ((buf[PACKET_LEN_OFFSET] << 8) | buf[PACKET_LEN_OFFSET + 1]);

    if ((datalen + PACKET_OVERHEAD) > *len) {
        // Not everything received yet
        return;
    }

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

    // Run callback
    bool cb_success = (*_callbacks[cmd])(buf + PACKET_DATA_OFFSET, &datalen);

    // Send response if necessary
    if (!is_broadcast) {
        buf[PACKET_DEST_OFFSET] = CONTROLLER_ADDR;
        buf[PACKET_SRC_OFFSET] = _addr;
        buf[PACKET_LEN_OFFSET] = 0x00;
        buf[PACKET_LEN_OFFSET + 1] = 0x00;
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

void MendeleevClass::tick()
{
    int i;
    static uint16_t buffPos = 0;

    // If the buffer is empty, wait until the data arrives.
    while (RS485Serial.available() > 0) {
        i = RS485Serial.read();
        if ((buffPos < PACKET_PREAMBLE_SIZE) && (i != PACKET_PREAMBLE)) {
            buffPos = 0;
            continue;
        }
        _dataBuffer[buffPos++] = i;
    }

    _parse(_dataBuffer, &buffPos);
}

/* ----------------------------------------------------------------------- */
/* LED methods. */
/* ----------------------------------------------------------------------- */
void MendeleevClass::setColor(uint8_t red, uint8_t green, uint8_t blue)
{
    analogWrite(LED_R_PIN, red);
    analogWrite(LED_G_PIN, green);
    analogWrite(LED_B_PIN, blue);
}

void MendeleevClass::setColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    setColor(red, green, blue);
    analogWrite(LED_A_PIN, alpha);
}

void MendeleevClass::setColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, uint8_t white)
{
    setColor(red, green, blue, alpha);
    analogWrite(LED_W_PIN, white);
}

void MendeleevClass::setTxt(uint8_t value)
{
    analogWrite(LED_TXT_PIN, value);
}

void MendeleevClass::setUv(uint8_t value)
{
    analogWrite(LED_UV_PIN, value);
}

// void MendeleevClass::setColorSpeed(uint8_t speed)
// {
//
// }

/* ----------------------------------------------------------------------- */
/* motor methods. */
/* ----------------------------------------------------------------------- */

// TODO

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
// #ifndef DEBUG
    uint8_t addr = 0;
    uint8_t value = 0;
    value = _mcp.digitalRead(DIPSW0_PIN);
    addr |= value;
    value = _mcp.digitalRead(DIPSW1_PIN);
    addr |= value << 1;
    value = _mcp.digitalRead(DIPSW2_PIN);
    addr |= value << 2;
    value = _mcp.digitalRead(DIPSW3_PIN);
    addr |= value << 3;
    value = _mcp.digitalRead(DIPSW4_PIN);
    addr |= value << 4;
    value = _mcp.digitalRead(DIPSW5_PIN);
    addr |= value << 5;
    value = _mcp.digitalRead(DIPSW6_PIN);
    addr |= value << 6;
    return addr;
// #else
//     return 2;
// #endif
}

uint8_t MendeleevClass::_getConfig()
{
    return _mcp.digitalRead(DIPSW7_PIN);
}
