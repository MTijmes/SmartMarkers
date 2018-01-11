#ifndef SPI_H
#define SPI_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stddef.h>

/* Public types --------------------------------------------------------------*/
/* Public constants ----------------------------------------------------------*/
/* Public macro --------------------------------------------------------------*/
/* Public functions --------------------------------------------------------- */
void spi_init(void);
void spi_write_buf(uint8_t addr, uint8_t *buf, size_t size);
void spi_read_buf(uint8_t addr, uint8_t *buf, size_t size);

#endif /* SPI_H */
