// Configuration parser implementation
// Based on VESC Express confparser

#include "confparser.h"
#include "buffer.h"
#include "datatypes.h"
#include <Arduino.h>


int32_t confparser_serialize_main_config_t(uint8_t *buffer, const main_config_t *conf) {
    int32_t ind = 0;

    // Signature
    buffer_append_uint32(buffer, MAIN_CONFIG_T_SIGNATURE, &ind);

    // controller_id (int16_t)
    buffer_append_int16(buffer, conf->controller_id, &ind);

    // can_baud_rate (uint8_t)
    buffer[ind++] = conf->can_baud_rate;

    // can_status_rate_hz (int16_t)
    buffer_append_int16(buffer, conf->can_status_rate_hz, &ind);

    return ind;
}

bool confparser_deserialize_main_config_t(const uint8_t *buffer, main_config_t *conf) {
    int32_t ind = 0;

    // Check signature
    uint32_t signature = buffer_get_uint32(buffer, &ind);
    if (signature != MAIN_CONFIG_T_SIGNATURE) {
        Serial.printf("[%lu] ⚠️ Config signature mismatch: got 0x%08X, expected 0x%08X\n", 
                     millis(), signature, MAIN_CONFIG_T_SIGNATURE);
        return false;
    }

    // controller_id (int16_t)
    conf->controller_id = buffer_get_int16(buffer, &ind);

    // can_baud_rate (uint8_t)
    conf->can_baud_rate = buffer[ind++];

    // can_status_rate_hz (int16_t)
    conf->can_status_rate_hz = buffer_get_int16(buffer, &ind);

    Serial.printf("[%lu] 📥 Deserialized config: ID=%d, Baud=%d, Rate=%d Hz\n",
                 millis(), conf->controller_id, conf->can_baud_rate, conf->can_status_rate_hz);

    return true;
}

void confparser_set_defaults_main_config_t(main_config_t *conf) {
    conf->controller_id = CONF_CONTROLLER_ID;
    conf->can_baud_rate = CONF_CAN_BAUD_RATE;
    conf->can_status_rate_hz = 20; // Default 20 Hz status rate
}
