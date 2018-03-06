#ifndef UBLOX_H
#define UBLOX_H

/* Includes ------------------------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <definitions.h>
#include <i2c.h>

/* Public types --------------------------------------------------------------*/
/* Public constants ----------------------------------------------------------*/
/* Public macro --------------------------------------------------------------*/
/* Public functions --------------------------------------------------------- */
void i2c_receive_nmea(uint32_t slave_address);
void parse_data(uint8_t marker_id, uint8_t *output);
void set_message(uint32_t slave_adress, uint8_t message_id, uint8_t rate);

#endif /* UBLOX_H */
