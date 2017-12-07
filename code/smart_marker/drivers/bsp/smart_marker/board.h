#ifndef BOARD_H
#define BOARD_H

/* Includes ------------------------------------------------------------------*/
#include <stm32l0xx_ll_bus.h>
#include <stm32l0xx_ll_rcc.h>
#include <stm32l0xx_ll_system.h>
#include <stm32l0xx_ll_utils.h>
#include <stm32l0xx_ll_cortex.h>
#include <stm32l0xx_ll_gpio.h>
#include <stm32l0xx_ll_exti.h>
#include <stm32l0xx_ll_usart.h>
#include <stm32l0xx_ll_i2c.h>
#include <stm32l0xx_ll_pwr.h>

#if defined(USE_FULL_ASSERT)
#include <stm32_assert.h>
#endif /* USE_FULL_ASSERT */

#define RECEIVE_SIZE 255

/* Public types --------------------------------------------------------------*/
/* Public constants ----------------------------------------------------------*/
/* Public macro --------------------------------------------------------------*/
/* Public functions --------------------------------------------------------- */
uint8_t i2c_receive_buffer[RECEIVE_SIZE];

void board_error_handler(void);
void board_irq_enable(void);
void board_irq_disable(void);
void board_init(void);
void board_deinit(void);

#endif /* BOARD_H */
