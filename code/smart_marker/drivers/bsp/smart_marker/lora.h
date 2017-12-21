#ifndef LORA_H
#define LORA_H

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <timer.h>
#include <LoRaMac.h>
#include <Region.h>

/* Public types --------------------------------------------------------------*/
/* Public define -------------------------------------------------------------*/
/* Public constants ----------------------------------------------------------*/
/* Public macro --------------------------------------------------------------*/
/* Public functions --------------------------------------------------------- */
bool lora_network_is_joined(void);

void lora_init(void);
void lora_join(void);
void lora_prepare_frame(uint8_t *buf, size_t size);
void lora_send_frame(void);

#endif /* LORA_H */
