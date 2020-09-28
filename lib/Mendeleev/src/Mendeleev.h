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

/* Debug defines */
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

#define BUFF_MAX (240)

/*
 * Commands
 */
enum Commands {
    COMMAND_SET_COLOR = 0x00,
    COMMAND_SET_MODE = 0x01,
    COMMAND_OTA = 0x02,
    COMMAND_GET_VERSION = 0x03,
    COMMAND_SET_OUTPUT = 0x04,
    COMMAND_REBOOT = 0x05,
    COMMAND_MAX = 0x06
};

/*
 * Inputs
 */
enum Input {
    INPUT_0,
    INPUT_1,
    INPUT_2,
    INPUT_3
};

/*
 * Outputs
 */
enum Output {
    OUTPUT_0,
    OUTPUT_1,
    OUTPUT_2,
    OUTPUT_3
};

/*
 * motor slots
 */
enum Motors {
    MOTOR_1,
    MOTOR_2
};

/*
 * motor types
 */
enum MotorType {
    MOTORTYPE_NONE, /* No motor board installed */
    MOTORTYPE_1,
    MOTORTYPE_2,
    MOTORTYPE_3
};

/*
 * modes
 */
enum Mode {
    MODE_OTA = 0x00,
    MODE_GUEST = 0x01,
    MODE_LECTURER = 0x02
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
    ELEMENT_Cp  = 112, // Copernicium
    ELEMENT_Nh  = 113, // Nihonium
    ELEMENT_Fl  = 114, // Flerovium
    ELEMENT_Mc  = 115, // Moscovium
    ELEMENT_Lv  = 116, // Livermorium
    ELEMENT_Ts  = 117, // Tennessine
    ELEMENT_Og  = 118, // Oganesson
    ELEMENT_MAX = 119
};

/*
 * Command callback function
 */
typedef bool (*CommandCallback)(uint8_t *data, uint16_t *len);

typedef struct _ledcolors {
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    uint16_t alpha;
    uint16_t white;
    uint16_t uv;
    uint16_t text;
    uint16_t motor1led;
    uint16_t motor2led;
} LedColors;

class MendeleevClass
{
public:
    /**
     * @brief Initialize Mendeleev board.
     *		  Micro controller Arduino Zero compatible, RS485 interface, LEDS
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

    /**
     * @brief Set the colors
     *
     * @param red The red color value
     * @param green The green color value
     * @param blue The blue color value
     *
     * @return void
     */
    void setColor(uint8_t red, uint8_t green, uint8_t blue);

    /**
     * @brief Fade the colors
     *
     * @param red The red color value
     * @param green The green color value
     * @param blue The blue color value
     *
     * @return void
     */
    void fadeColor(uint8_t red, uint8_t green, uint8_t blue);

    /**
     * @brief Set the colors
     *
     * @param red The red color value
     * @param green The green color value
     * @param blue The blue color value
     * @param alpha The alpha color value
     *
     * @return void
     */
    void setColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);

    /**
     * @brief Fade the colors
     *
     * @param red The red color value
     * @param green The green color value
     * @param blue The blue color value
     * @param alpha The alpha color value
     *
     * @return void
     */
    void fadeColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);

    /**
     * @brief Set the colors
     *
     * @param red The red color value
     * @param green The green color value
     * @param blue The blue color value
     * @param alpha The alpha color value
     * @param white The white color value
     *
     * @return void
     */
    void setColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, uint8_t white);

    /**
     * @brief Fade the colors
     *
     * @param red The red color value
     * @param green The green color value
     * @param blue The blue color value
     * @param alpha The alpha color value
     * @param white The white color value
     *
     * @return void
     */
    void fadeColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, uint8_t white);

    /**
     * @brief Set the colors
     *
     * @param red The red color value
     * @param green The green color value
     * @param blue The blue color value
     * @param alpha The alpha color value
     * @param white The white color value
     * @param uv The uv color value
     * @param txt The txt color value
     *
     * @return void
     */
    void setColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, uint8_t white, uint8_t uv, uint8_t txt);

    /**
     * @brief Fade the colors
     *
     * @param red The red color value
     * @param green The green color value
     * @param blue The blue color value
     * @param alpha The alpha color value
     * @param white The white color value
     * @param uv The uv color value
     * @param txt The txt color value
     *
     * @return void
     */
    void fadeColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, uint8_t white, uint8_t uv, uint8_t txt);

    /**
     * @brief Set the txt value
     *
     * @param value The txt value
     *
     * @return void
     */
    void setTxt(uint8_t value);

    /**
     * @brief Fade the txt value
     *
     * @param value The txt value
     *
     * @return void
     */
    void fadeTxt(uint8_t value);

    /**
     * @brief Set the uv value
     *
     * @param value The uv value
     *
     * @return void
     */
    void setUv(uint8_t value);

    /**
     * @brief Fade the uv value
     *
     * @param value The uv value
     *
     * @return void
     */
    void fadeUv(uint8_t value);

    /**
     * @brief Set the motor led value
     *
     * @param motor the motor to light up
     * @param value The uv value
     *
     * @return void
     */
    void setMotorLed(enum Motors motor, uint8_t value);

    /**
     * @brief Fade the motor led value
     *
     * @param motor the motor to light up
     * @param value The uv value
     *
     * @return void
     */
    void fadeMotorLed(enum Motors motor, uint8_t value);

    // TODO: make fading speed adjustable?
    // void setFadingSpeed(uint16_t speedTime);

    /* ----------------------------------------------------------------------- */
    /* motor methods. */
    /* ----------------------------------------------------------------------- */

    /**
     * @brief Get the type of the motor
     *
     * @param motor The motor to get the type from
     *
     * @return enum MotorType the type of the motor
     */
    enum MotorType getMotorType(enum Motors motor); // TODO do we need this publicly?

    /* ----------------------------------------------------------------------- */
    /* input/Output methods. */
    /* ----------------------------------------------------------------------- */

    /**
     * @brief Read the state of an input
     *
     * @param input The input to get the value from
     *
     * @return int the value of the input
     */
    uint8_t getInput(enum Input input);

    /**
     * @brief Attach an interrupt handler to an input
     *
     * @param input Wait delay if not available to read byte in milliseconds. Default 10.
     * @param repeatTime Repeat time if not read bytes. Default 10. All time = delayWait * repeatTime.
     *
     * @return void
     */
    void attachInputInterrupt(enum Input input, voidFuncPtr ISR, int mode);

    /**
     * @brief Attach an interrupt to the proximity input
     *
     * @param delayWait Wait delay if not available to read byte in milliseconds. Default 10.
     * @param repeatTime Repeat time if not read bytes. Default 10. All time = delayWait * repeatTime.
     *
     * @return void
     */
    void attachProximityInterrupt(voidFuncPtr ISR, int mode);

    /**
     * @brief Set the state of the outputs
     *
     * @param output Which output to set.
     * @param value the value to set the output to.
     *
     * @return void
     */
    void setOutput(enum Output output, uint8_t value);

    /**
     * @brief Heartbeat. Reads serial data and takes care of timeouts and fading.
     *
     * @return void
     */
    void tick();

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
    void startAnimation();

    /**
     * @brief Set the current mode
     *
     * @param mode The mode to set
     *
     * @return void
     */
    void setMode(enum Mode mode);

    /**
     * @brief Get the configured address
     *
     * @return int address
     */
    uint8_t getAddress();

protected:
    uint8_t _addr;                                ///< the address of this board
    enum MotorType _slot1Type;                    ///< the type of the motor in slot 1
    enum MotorType _slot2Type;                    ///< the type of the motor in slot 2
    CommandCallback _callbacks[COMMAND_MAX];      ///< the array of callback functions
    enum Mode _current_mode;                      ///< the current mode

private:
    uint8_t _motor_direction = HIGH;
    uint8_t _dataBuffer[BUFF_MAX];
    Adafruit_MCP23017 _mcp;

    LedColors _current_colors;                    /* R, G, B, A, W, UV, TXT, Motor 1 LED, Motor 2 LED */
    LedColors _initial_colors;                    // Used when fading.
    LedColors _target_colors;                     // Used when fading.
    uint16_t _fading_step;                        // Current step of the fading.
    uint16_t _fading_max_steps;                   // The total number of steps when fading.
    uint16_t _fading_step_time;                   // The number of ms between two variation of color when fading.
    bool _fading;                                 // Are we fading now ?
    unsigned long _last_update;                   // Last time we did something.
    void _fade();                                 // Used internaly to fade

    uint8_t _getConfig();                         // Get the value of the config pin
    void _parse(uint8_t *buf, uint16_t* len);     // Parse the incoming data
    uint16_t _crc16(uint8_t *buf, uint16_t len);  // Calculate the checksum
    void _stopAnimation();                        // Stop the animation
    bool _animating;                              // Is the animation currently running?
    unsigned long _animationStartTime;            // animation start time
    unsigned long _modeStartTime;                 // lecturer mode start time
};

extern MendeleevClass Mendeleev;

#endif // _MENDELEEV_H
