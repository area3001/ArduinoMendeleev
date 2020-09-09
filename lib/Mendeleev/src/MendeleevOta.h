#ifndef _MENDELEEV_OTA_H
#define _MENDELEEV_OTA_H

#include <Arduino.h>
#include <InternalStorage.h>

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

/*
 * OTA status
 */
enum OtaStatus {
    STATE_IDLE = 0,
    STATE_INPROGRESS,
    STATE_READY
};

enum OtaError {
    ERROR_NO_ERROR = 0,
    ERROR_DONE = 1,
    ERROR_NOT_READY,
    ERROR_INVALID_INDEX,
    ERROR_INVALID_SIZE,
    ERROR_TIMEOUT,
    ERROR_TOO_LARGE
};

class MendeleevOtaClass
{
public:
    /**
     * @brief Initialize the OTA class
     *
     * @return void
     */
    void init();

    /**
     * @brief Write OTA fragments
     *
     * @param index the fragment index.
     * @param data The fragment data
     * @param datalen The fragment data length
     *
     * @return enum OtaError
     */
    enum OtaError write(uint8_t index, uint8_t *data, uint16_t datalen);

    /**
     * @brief Tick method to check if an update needs to be applied or canceled
     *
     * @return void
     */
    void tick();

private:
    uint8_t _expected_idx;          /* Counter indicating OTA progress */
    uint32_t _update_file_size;     /* size of the update file */
    enum OtaStatus _current_state;  /* flag to trigger an update */
    unsigned long _last_update;     /* last time an OTA message was received and processed */
};

extern MendeleevOtaClass MendeleevOta;

#endif // _MENDELEEV_OTA_H
