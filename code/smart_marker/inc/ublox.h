#ifndef ublox_h
#define ublox_h
#include <stdio.h>
#include <definitions.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <i2c.h>
void i2c_receive_nmea(uint32_t slave_address);
void parse_data(uint8_t marker_id, uint8_t *output);
void set_message(uint32_t slave_adress, uint8_t message_id, uint8_t rate);

#endif //ublox_h