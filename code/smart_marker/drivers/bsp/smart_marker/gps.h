#ifndef GPS_H
#define GPS_H

/* Includes ------------------------------------------------------------------*/
#include <stddef.h>

/* Public types --------------------------------------------------------------*/
struct gps_data {
    uint8_t *buf;
    size_t size;
};

/* Public constants ----------------------------------------------------------*/
/* Public macro --------------------------------------------------------------*/
/* Public functions --------------------------------------------------------- */
void gps_init(struct gps_data *gps_data);
void gps_start(void);
void gps_stop(void);
void gps_read(struct gps_data *gps_data);

#endif /* GPS_H */
