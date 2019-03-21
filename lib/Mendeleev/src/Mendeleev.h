/*
 * Mendeleev.h
 * Company: Area 3001
 * Web: https://area3001.com
 * Description:
 * 		Library for Mendeleev board
 * Version: 1.0.0
 * Date: 10/2/2019
 * Author: Bert Outtier <outtierbert@gmail.com>
 */

#ifndef _MENDELEEV_H
#define _MENDELEEV_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <Adafruit_MCP23017.h>

#define BUFF_MAX (0xFF)

/*
 * Commands
 */
enum Commands {
    COMMAND_SET_COLOR = 0x00,
    COMMAND_SET_MODE = 0x01,
    COMMAND_OTA = 0x02,
    COMMAND_GET_VERSION = 0x03,
    COMMAND_SET_OUTPUT = 0x04,
    COMMAND_MAX = 0x05
};

/*
 * Input enum
 */
enum Input {
    INPUT_0,
    INPUT_1,
    INPUT_2,
    INPUT_3
};

/*
 * Output enum
 */
enum Output {
    OUTPUT_0,
    OUTPUT_1,
    OUTPUT_2,
    OUTPUT_3
};

/*
 * motor slot
 */
enum Motors {
    MOTOR_1,
    MOTOR_2
};

/*
 * motor types
 */
enum MotorType {
    MOTORTYPE_0, /* No motor board installed */
    MOTORTYPE_1,
    MOTORTYPE_2,
    MOTORTYPE_3
};

/*
 * Periodic table elements
 */
enum PeriodicElement {
    ELEMENT_H   =   1, // Hydrogen
    ELEMENT_He  =   2, // Helium
    ELEMENT_Li  =   3, // Lithium
    ELEMENT_Be  =   4, // Beryllium
    ELEMENT_B   =   5, // Boron
    ELEMENT_C   =   6, // Carbon
    ELEMENT_N   =   7, // Nitrogen
    ELEMENT_O   =   8, // Oxygen
    ELEMENT_F   =   9, // Fluorine
    ELEMENT_Ne  =  10, // Neon
    ELEMENT_Na  =  11, // Sodium
    ELEMENT_Mg  =  12, // Magnesium
    ELEMENT_Al  =  13, // Aluminum
    ELEMENT_Si  =  14, // Silicon
    ELEMENT_P   =  15, // Phosphorus
    ELEMENT_S   =  16, // Sulfur
    ELEMENT_Cl  =  17, // Chlorine
    ELEMENT_Ar  =  18, // Argon
    ELEMENT_K   =  19, // Potassium
    ELEMENT_Ca  =  20, // Calcium
    ELEMENT_Sc  =  21, // Scandium
    ELEMENT_Ti  =  22, // Titanium
    ELEMENT_V   =  23, // Vanadium
    ELEMENT_Cr  =  24, // Chromium
    ELEMENT_Mn  =  25, // Manganese
    ELEMENT_Fe  =  26, // Iron
    ELEMENT_Co  =  27, // Cobalt
    ELEMENT_Ni  =  28, // Nickel
    ELEMENT_Cu  =  29, // Copper
    ELEMENT_Zn  =  30, // Zinc
    ELEMENT_Ga  =  31, // Gallium
    ELEMENT_Ge  =  32, // Germanium
    ELEMENT_As  =  33, // Arsenic
    ELEMENT_Se  =  34, // Selenium
    ELEMENT_Br  =  35, // Bromine
    ELEMENT_Kr  =  36, // Krypton
    ELEMENT_Rb  =  37, // Rubidium
    ELEMENT_Sr  =  38, // Strontium
    ELEMENT_Y   =  39, // Yttrium
    ELEMENT_Zr  =  40, // Zirconium
    ELEMENT_Nb  =  41, // Niobium
    ELEMENT_Mo  =  42, // Molybdenum
    ELEMENT_Tc  =  43, // Technetium
    ELEMENT_Ru  =  44, // Ruthenium
    ELEMENT_Rh  =  45, // Rhodium
    ELEMENT_Pd  =  46, // Palladium
    ELEMENT_Ag  =  47, // Silver
    ELEMENT_Cd  =  48, // Cadmium
    ELEMENT_In  =  49, // Indium
    ELEMENT_Sn  =  50, // Tin
    ELEMENT_Sb  =  51, // Antimony
    ELEMENT_Te  =  52, // Tellurium
    ELEMENT_I   =  53, // Iodine
    ELEMENT_Xe  =  54, // Xenon
    ELEMENT_Cs  =  55, // Cesium
    ELEMENT_Ba  =  56, // Barium
    ELEMENT_La  =  57, // Lanthanum
    ELEMENT_Ce  =  58, // Cerium
    ELEMENT_Pr  =  59, // Praseodymium
    ELEMENT_Nd  =  60, // Neodymium
    ELEMENT_Pm  =  61, // Promethium
    ELEMENT_Sm  =  62, // Samarium
    ELEMENT_Eu  =  63, // Europium
    ELEMENT_Gd  =  64, // Gadolinium
    ELEMENT_Tb  =  65, // Terbium
    ELEMENT_Dy  =  66, // Dysprosium
    ELEMENT_Ho  =  67, // Holmium
    ELEMENT_Er  =  68, // Erbium
    ELEMENT_Tm  =  69, // Thulium
    ELEMENT_Yb  =  70, // Ytterbium
    ELEMENT_Lu  =  71, // Lutetium
    ELEMENT_Hf  =  72, // Hafnium
    ELEMENT_Ta  =  73, // Tantalum
    ELEMENT_W   =  74, // Tungsten
    ELEMENT_Re  =  75, // Rhenium
    ELEMENT_Os  =  76, // Osmium
    ELEMENT_Ir  =  77, // Iridium
    ELEMENT_Pt  =  78, // Platinum
    ELEMENT_Au  =  79, // Gold
    ELEMENT_Hg  =  80, // Mercury
    ELEMENT_Tl  =  81, // Thallium
    ELEMENT_Pb  =  82, // Lead
    ELEMENT_Bi  =  83, // Bismuth
    ELEMENT_Po  =  84, // Polonium
    ELEMENT_At  =  85, // Astatine
    ELEMENT_Rn  =  86, // Radon
    ELEMENT_Fr  =  87, // Francium
    ELEMENT_Ra  =  88, // Radium
    ELEMENT_Ac  =  89, // Actinium
    ELEMENT_Th  =  90, // Thorium
    ELEMENT_Pa  =  91, // Protactinium
    ELEMENT_U   =  92, // Uranium
    ELEMENT_Np  =  93, // Neptunium
    ELEMENT_Pu  =  94, // Plutonium
    ELEMENT_Am  =  95, // Americium
    ELEMENT_Cm  =  96, // Curium
    ELEMENT_Bk  =  97, // Berkelium
    ELEMENT_Cf  =  98, // Californium
    ELEMENT_Es  =  99, // Einsteinium
    ELEMENT_Fm  = 100, // Fermium
    ELEMENT_Md  = 101, // Mendelevium
    ELEMENT_No  = 102, // Nobelium
    ELEMENT_Lr  = 103, // Lawrencium
    ELEMENT_Rf  = 104, // Rutherfordium
    ELEMENT_Db  = 105, // Dubnium
    ELEMENT_Sg  = 106, // Seaborgium
    ELEMENT_Bh  = 107, // Bohrium
    ELEMENT_Hs  = 108, // Hassium
    ELEMENT_Mt  = 109, // Meitnerium
    ELEMENT_Ds  = 110, // Darmstadtium
    ELEMENT_Rg  = 111, // Roentgenium
    ELEMENT_Uub = 112, // Ununbium
    ELEMENT_Uut = 113, // Ununtrium
    ELEMENT_Uuq = 114, // Ununquadium
    ELEMENT_Uup = 115, // Ununpentium
    ELEMENT_Uuh = 116, // Ununhexium
    ELEMENT_Uus = 117, // Ununseptium
    ELEMENT_Uuo = 118, // Oganesson
    ELEMENT_MAX = 119
};

typedef bool (*CommandCallback)(uint8_t *data, uint16_t *len);

class MendeleevClass
{
public:
    /**
     * @brief Initialize Mendeleev board.
     *		  Micro controller Arduino Zero compatible, RS485 interface, LEDS and???.=
     *
     * @return void
     */
    void init();

    /**
     * @brief Connect to RS485. With default configuration SERIAL_8N1.
     *
     * @param baud Speed.
     *     Values: 75, 110, 300, 1200, 2400, 4800, 9600, 19200, 38400, 57600 and 115200 bit/s.
     *
     * @return void
     */
    void RS485Begin(unsigned long baud);

    /**
     * @brief Start connect to RS485.
     *
     * @param baud Speed.
     *             Values: 75, 110, 300, 1200, 2400, 4800, 9600, 19200, 38400, 57600 and 115200 bit/s.
     * @param config Configuration - data bits, parity, stop bits.
     *               Values: SERIAL_5N1, SERIAL_6N1, SERIAL_7N1, SERIAL_8N1, SERIAL_5N2, SERIAL_6N2, SERIAL_7N2, SERIAL_8N2, SERIAL_5E1, SERIAL_6E1, SERIAL_7E1, SERIAL_8E1, SERIAL_5E2,
     SERIAL_6E2, SERIAL_7E2, SERIAL_8E2, SERIAL_5O1, SERIAL_6O1, SERIAL_7O1, SERIAL_8O1, SERIAL_5O2, SERIAL_6O2, SERIAL_7O2, SERIAL_8O2
     *
     * @return void
    */
    void RS485Begin(unsigned long baud, uint16_t config);

    /**
     * @brief Close connection to RS485.
     *
     * @return void
     */
    void RS485End();

    /**
     * @brief Register a callback function for a command
     *
     * @return void
     */
    void registerCallback(enum Commands command, CommandCallback cb);

    /**
     * @brief Unregister a callback function for a command
     *
     * @return void
     */
    void unregisterCallback(enum Commands command);
    /**
     * @brief Transmit one byte data to RS485.
     *
     * @param data Transmit data.
     *
     * @return size_t Count of transmitted - one byte.
     */
    size_t RS485Write(uint8_t data);

    /**
     * @brief Transmit one char data to RS485.
     *
     * @param data Transmit data.
     *
     * @return size_t Count of transmitted - one char.
     */
    size_t RS485Write(char data);

    /**
     * @brief Transmit the text to RS485.
     *
     * @param data Text data to transmit.
     *
     * @return size_t Count of transmitted chars.
     */
    size_t RS485Write(const char* data);

    /**
     * @brief Transmit the text to RS485.
     *
     * @param data Text data to transmit.
     *
     * @return size_t Count of transmitted chars.
     */
    size_t RS485Write(String data) { return RS485Write(data.c_str()); }

    /**
     * @brief Send array of bytes to RS485.
     *
     * @param data Array in bytes to be send.
     * @param dataLen Array length.
     *
     * @return size_t Count of transmitted bytes.
     */
    size_t RS485Write(uint8_t* data, uint8_t dataLen);

    /**
     * @brief Read received data from RS485.
     *
     *
     * @return int Received byte.<para></para>
     *   If result = -1 - buffer is empty, no data
     *   if result > -1 - valid byte to read.
     */
    int RS485Read();

    /**
     * @brief Read received data from RS485. Reading data with delay and repeating the operation while all data to arrive.
     *
     * @param delayWait Wait delay if not available to read byte in milliseconds. Default 10.
     * @param repeatTime Repeat time if not read bytes. Default 10. All time = delayWait * repeatTime.
     *
     * @return int Received byte.
     *   If result = -1 - buffer is empty, no data<para></para>
     *   if result > -1 - valid byte to read.
     */
    int RS485Read(unsigned long delayWait, uint8_t repeatTime);

    /* ----------------------------------------------------------------------- */
    /* LED methods. */
    /* ----------------------------------------------------------------------- */
    void setColor(uint8_t red, uint8_t green, uint8_t blue);

    void setColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);

    void setColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, uint8_t white);

    void setTxt(uint8_t value);

    void setUv(uint8_t value);

    // TODO: fade to color functions?
    // void setColorSpeed(uint8_t speed);

    /* ----------------------------------------------------------------------- */
    /* motor methods. */
    /* ----------------------------------------------------------------------- */
    enum MotorType getMotorType(enum Motors motor); // TODO do we need this publicly?

    void setMotorCtrl(enum Motors motor); // TODO what is this?

    /* ----------------------------------------------------------------------- */
    /* input/Output methods. */
    /* ----------------------------------------------------------------------- */
    uint8_t getInput(enum Input input);

    void attachInputInterrupt(enum Input input, voidFuncPtr ISR, int mode);

    void attachProximityInterrupt(voidFuncPtr ISR, int mode);

    void setOutput(enum Output output, uint8_t value);

    // heartbeat
    void tick();

protected:
    uint8_t _addr; ///< the address of this board
    enum MotorType _slot1Type;
    enum MotorType _slot2Type;
    CommandCallback _callbacks[COMMAND_MAX]; ///< the array of callback functions

private:
    uint8_t _dataBuffer[BUFF_MAX];
    Adafruit_MCP23017 _mcp;
    uint8_t _getAddress();
    uint8_t _getConfig();
    void _parse(uint8_t *buf, uint16_t* len);
    uint16_t _crc16(uint8_t *buf, uint16_t len);
};

extern MendeleevClass Mendeleev;

#endif // _MENDELEEV_H
