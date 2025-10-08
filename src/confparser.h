// Configuration parser for main_config_t
// Based on VESC Express confparser

#ifndef CONFPARSER_H_
#define CONFPARSER_H_

#include "datatypes.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Functions
int32_t confparser_serialize_main_config_t(uint8_t *buffer, const main_config_t *conf);
bool confparser_deserialize_main_config_t(const uint8_t *buffer, main_config_t *conf);
void confparser_set_defaults_main_config_t(main_config_t *conf);

#ifdef __cplusplus
}
#endif

#endif // CONFPARSER_H_
