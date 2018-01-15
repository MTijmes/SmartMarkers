#ifndef UART_H
#define UART_H

/* Includes ------------------------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>

/* Public types --------------------------------------------------------------*/
/* Public constants ----------------------------------------------------------*/
/* Public macro --------------------------------------------------------------*/
/* Public functions --------------------------------------------------------- */
void uart_get_char(uint8_t *data);
void uart_put_char(uint8_t data);
void uart_get_buffer(uint8_t *buf, size_t size);
void uart_put_buffer(uint8_t *buf, size_t size);

void uart_init(void);

#endif /* UART_H */
