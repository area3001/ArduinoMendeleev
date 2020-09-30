#include "MendeleevOta.h"

const unsigned long OTA_TIMEOUT(5000); /* Timeout of OTA */

MendeleevOtaClass MendeleevOta;

void MendeleevOtaClass::init()
{
    _expected_idx = 0;             /* Counter indicating OTA progress */
    _update_file_size = 0;         /* file size of the update file */
    _current_state = STATE_IDLE;   /* flag to trigger an update */
    _last_update = 0;              /* last time an OTA message was received and processed */
}

enum OtaError MendeleevOtaClass::write(uint8_t index, uint8_t *data, uint16_t datalen)
{
    enum OtaError ret = ERROR_NO_ERROR;

    switch(_current_state) {
    case STATE_IDLE:
        _expected_idx = 0;
        _update_file_size = 0;
        if (index != 0) {
            DEBUG_PRINTLN("OTA: Expected index 0");
            ret = ERROR_INVALID_INDEX;
        }
        else {
            if (datalen != 4) {
                DEBUG_PRINTLN("OTA: Expected a 4 byte payload size");
                ret = ERROR_INVALID_SIZE;
            }
            else {
                _update_file_size = ((data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]);
                DEBUG_PRINT("OTA: update size is "); DEBUG_PRINTDEC(_update_file_size); DEBUG_PRINTLN(" bytes");
                if (((long)_update_file_size) > InternalStorage.maxSize()) {
                    DEBUG_PRINTLN("OTA: file too large");
                    ret = ERROR_TOO_LARGE;
                }
                else {
                    DEBUG_PRINTLN("OTA: start update");
                    _expected_idx = 1;
                    _current_state = STATE_INPROGRESS;
                    InternalStorage.open(_update_file_size);
                }
            }
        }
        break;
    case STATE_INPROGRESS:
        if (index != _expected_idx) {
            DEBUG_PRINT("OTA: expected idx is ");
            DEBUG_PRINTDEC(_expected_idx);
            DEBUG_PRINT(", but received ");
            DEBUG_PRINTDEC(index);
            DEBUG_PRINTLN("!");
            InternalStorage.close();
            InternalStorage.clear();
            _current_state = STATE_IDLE;
            ret = ERROR_INVALID_INDEX;
        }
        else {
            int i = 0;
            while (datalen > 0 && _update_file_size > 0) {
                InternalStorage.write((char)data[i++]);
                _update_file_size--;
                datalen--;
            }
            if (datalen > 0) {
                DEBUG_PRINTLN("OTA: too much data");
                InternalStorage.close();
                InternalStorage.clear();
                _current_state = STATE_IDLE;
                ret = ERROR_INVALID_SIZE;
            }
            else if (_update_file_size > 0) {
                _expected_idx = index + 1;
            }
            else {
                DEBUG_PRINTLN("OTA: all data received");
                _current_state = STATE_READY;
                InternalStorage.close();
            }
        }
        break;
    case STATE_READY:
    default:
        DEBUG_PRINTLN("OTA: We should not get here");
        // If you still write in this state
        InternalStorage.clear();
        _current_state = STATE_IDLE;
        ret = ERROR_INVALID_INDEX;
    }

    _last_update = millis();
    return ret;
}

bool MendeleevOtaClass::tick()
{
    if (_current_state == STATE_INPROGRESS) {
        if ((long) (millis() - _last_update) >= OTA_TIMEOUT) {
            DEBUG_PRINTLN("OTA: timeout");
            InternalStorage.close();
            InternalStorage.clear();
            _current_state = STATE_IDLE;
        }
        else {
            return true;
        }
    }
    return false;
}

enum OtaStatus MendeleevOtaClass::state()
{
    return _current_state;
}

void MendeleevOtaClass::apply()
{
    if (_current_state == STATE_READY) {
        DEBUG_PRINTLN("OTA: Executing firmware update");
        InternalStorage.apply();
        while (true);
    }
}