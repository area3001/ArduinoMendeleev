// Mendeleev.cpp
// Company: Area 3001
// Web: https://area3001.com
// Description:
//		Library for Meneleev board
// Version: 1.1.0
// Date: 10/2/2019
// Author: Bert Outtier <outtierbert@gmail.com>

#include "Mendeleev.h"

// RS485 Pins are set to sercom5 by default
#define RS485_DIR_PIN (19)  // A5/PB02
#define RS485Serial	  Serial1 // Sercom5 (uses UART.RXD/PB23, UART.TXD/PB22)
#define RS485Transmit HIGH
#define RS485Receive  LOW

// i2C pin are set to I2C by default
// #define I2C_SCL_PIN (20) // I2C.SCL/PA22
// #define I2C_SDA_PIN (21) // I2C.SDA/PA23

// SPI pins are set to SPI by default
// #define SPI_MISO_PIN (22) // SPI.MISO/PA12
// #define SPI_MOSI_PIN (23) // SPI.MOSI/PB10
// #define SPI_SCLK_PIN (24) // SPI.SCLK/PB11
#define SPI_CS0_PIN  (2)  // SPI.CS0/D2/PA14
#define SPI_CS1_PIN  (39) // SPI.CS1/D7/PA21

// Motors
#define M1CTRL0_PIN (0)  // D0/PA11
#define M1CTRL1_PIN (41) // D9/PA07
#define M1CTRL2_PIN (40) // D8/PA06
#define M2CTRL0_PIN (1)  // D1/PA10
#define M2CTRL1_PIN (3)  // D3/PA09
#define M2CTRL2_PIN (4)  // D4/PA08

// Inputs
#define INPUT0_PIN (15) // A1/PB08
#define INPUT1_PIN (16) // A2/PB09
#define INPUT2_PIN (14) // A0/PA02
#define INPUT3_PIN (17) // A3/PA04

// Proximity
#define PROX_PIN (18) // A4/PA05

// Dip switch pins (connected to MCP23017)
#define DIPSW0_PIN (0)
#define DIPSW1_PIN (1)
#define DIPSW2_PIN (2)
#define DIPSW3_PIN (3)
#define DIPSW4_PIN (4)
#define DIPSW5_PIN (5)
#define DIPSW6_PIN (6)
#define DIPSW7_PIN (7)

// Output pins (connected to MCP23017)
#define OUTPUT0_PIN (8)
#define OUTPUT1_PIN (9)
#define OUTPUT2_PIN (10)
#define OUTPUT3_PIN (11)

// Motor type pins
#define M1TYPE0_PIN (12)
#define M1TYPE1_PIN (13)
#define M2TYPE0_PIN (14)
#define M2TYPE1_PIN (15)

// LED pins
#define LED_R_PIN   (5)  // D5/PA15
#define LED_G_PIN   (11) // D11/PA16
#define LED_B_PIN   (13) // D13/PA17
#define LED_A_PIN   (10) // D10/PA18
#define LED_W_PIN   (12) // D12/PA19
#define LED_UV_PIN  (6)  // D6/PA20
#define LED_TXT_PIN (38) // LED TXT/PA13

MendeleevClass Mendeleev;

void MendeleevClass::init()
{
    // RS485 pin init.
    pinMode(RS485_DIR_PIN, OUTPUT);
    digitalWrite(RS485_DIR_PIN, RS485Receive);

    // Set up SPI CS
    pinMode(SPI_CS0_PIN, OUTPUT);
    pinMode(SPI_CS1_PIN, OUTPUT);

    // Set up motor control pins
    pinMode(M1CTRL0_PIN, OUTPUT);
    pinMode(M1CTRL1_PIN, OUTPUT);
    pinMode(M1CTRL2_PIN, OUTPUT);
    pinMode(M2CTRL0_PIN, OUTPUT);
    pinMode(M2CTRL1_PIN, OUTPUT);
    pinMode(M2CTRL2_PIN, OUTPUT);

    // Input pins init
    pinMode(INPUT0_PIN, INPUT);
    pinMode(INPUT1_PIN, INPUT);
    pinMode(INPUT2_PIN, INPUT);
    pinMode(INPUT3_PIN, INPUT);

    // Proximity pin init
    pinMode(PROX_PIN, INPUT);

    _mcp.begin();

    // Setup pins of MCP
    _mcp.pinMode(DIPSW0_PIN, INPUT);
    _mcp.pinMode(DIPSW1_PIN, INPUT);
    _mcp.pinMode(DIPSW2_PIN, INPUT);
    _mcp.pinMode(DIPSW3_PIN, INPUT);
    _mcp.pinMode(DIPSW4_PIN, INPUT);
    _mcp.pinMode(DIPSW5_PIN, INPUT);
    _mcp.pinMode(DIPSW6_PIN, INPUT);
    _mcp.pinMode(DIPSW7_PIN, INPUT);

    // Read our address
    _addr = _getAddress();

    _mcp.pinMode(OUTPUT0_PIN, OUTPUT);
    _mcp.pinMode(OUTPUT1_PIN, OUTPUT);
    _mcp.pinMode(OUTPUT2_PIN, OUTPUT);
    _mcp.pinMode(OUTPUT3_PIN, OUTPUT);

    _mcp.pinMode(M1TYPE0_PIN, INPUT);
    _mcp.pinMode(M1TYPE1_PIN, INPUT);
    _mcp.pinMode(M2TYPE0_PIN, INPUT);
    _mcp.pinMode(M2TYPE1_PIN, INPUT);

    // Set up LED pins
    pinMode(LED_R_PIN,   OUTPUT);
    pinMode(LED_G_PIN,   OUTPUT);
    pinMode(LED_B_PIN,   OUTPUT);
    pinMode(LED_A_PIN,   OUTPUT);
    pinMode(LED_W_PIN,   OUTPUT);
    pinMode(LED_UV_PIN,  OUTPUT);
    pinMode(LED_TXT_PIN, OUTPUT);
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

// uint16_t MendeleevClass::_calculate_checksum(uint8_t *buf, uint8_t len)
// {
//   uint16_t i;
//   uint16_t chksm = 0;
//   for (i=0; i<len; i++) {
//     chksm += buf[i];
//   }
//   return chksm;
// }

uint16_t MendeleevClass::_calculate_checksum(uint8_t *buf, uint8_t len)
{
    uint16_t temp, temp2, flag;
    temp = 0xFFFF;
    for (uint8_t i = 0; i < len; i++)
    {
        temp = temp ^ buf[i];
        for (uint8_t j = 1; j <= 8; j++)
        {
            flag = temp & 0x0001;
            temp >>= 1;
            if (flag)
            temp ^= 0xA001;
        }
    }
    // Reverse byte order.
    temp2 = temp >> 8;
    temp = (temp << 8) | temp2;
    temp &= 0xFFFF;
    // the returned value is already swapped
    // crcLo byte is first & crcHi byte is last
    return temp;
}

void MendeleevClass::_parse(uint8_t *buf, uint16_t *len)
{
    if (*len < PACKET_OVERHEAD) {
        // The packet is not complete yet
        return;
    }

    if (buf[PACKET_DEST_OFFSET] != _addr && buf[PACKET_DEST_OFFSET] != BROADCAST_ADDR) {
        // not for me, throw away
        *len = 0;
        return;
    }

    bool is_broadcast = (buf[PACKET_DEST_OFFSET] == BROADCAST_ADDR);

    if (buf[PACKET_CMD_OFFSET] >= COMMAND_MAX) {
        // invalid command
        *len = 0;
        return;
    }

    enum Commands cmd = (enum Commands)buf[PACKET_CMD_OFFSET];

    if (_callbacks[cmd] == NULL) {
        // No cllback for this command
        *len = 0;
        return;
    }

    uint16_t datalen = *((uint16_t *)(buf + PACKET_LEN_OFFSET));

    if ((datalen + PACKET_OVERHEAD) > *len) {
        // Not everything received yet
        return;
    }

    if (*((uint16_t *)(buf + PACKET_DATA_OFFSET + datalen)) != _calculate_checksum(buf, PACKET_DATA_OFFSET + datalen)) {
        // incorrect checksum
        *len = 0;
        return;
    }

    // Run callback
    bool cb_success = (*_callbacks[cmd])(buf + PACKET_DATA_OFFSET, datalen);

    // Send response if necessary
    if (!is_broadcast) {
        buf[PACKET_DEST_OFFSET] = CONTROLLER_ADDR;
        buf[PACKET_SRC_OFFSET] = _addr;
        *((uint16_t *)(buf + PACKET_LEN_OFFSET)) = 0x0000;
        if (!cb_success) {
            buf[PACKET_CMD_OFFSET] = ~buf[PACKET_CMD_OFFSET];
        }
        buf[PACKET_DATA_OFFSET] = _calculate_checksum(buf, PACKET_DATA_OFFSET);
        RS485Write(buf, PACKET_OVERHEAD);
    }
}

void MendeleevClass::tick()
{
    int i;
    static uint16_t buffPos = 0;

    // If the buffer is empty, wait until the data arrives.
    while (RS485Serial.available()) {
        i = RS485Serial.read();
        if (i == -1) {
            break;
        }
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
        return LOW; // TODO: Log error somewhere
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
    }
}

/* ----------------------------------------------------------------------- */
/* Private methods. */
/* ----------------------------------------------------------------------- */
uint8_t MendeleevClass::_getAddress()
{
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
}

uint8_t MendeleevClass::_getConfig()
{
    return _mcp.digitalRead(DIPSW7_PIN);
}
