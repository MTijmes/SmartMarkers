#ifndef BOARD_H
#define BOARD_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Public types --------------------------------------------------------------*/
/* Public constants ----------------------------------------------------------*/
#define RADIO_RESET_PORT        GPIOB
#define RADIO_RESET_PIN         LL_GPIO_PIN_8
#define RADIO_RESET_CLK         LL_IOP_GRP1_PERIPH_GPIOB

#define RADIO_MOSI_PORT         GPIOA
#define RADIO_MOSI_PIN          LL_GPIO_PIN_7
#define RADIO_MISO_PORT         GPIOA
#define RADIO_MISO_PIN          LL_GPIO_PIN_6
#define RADIO_SCLK_PORT         GPIOA
#define RADIO_SCLK_PIN          LL_GPIO_PIN_5
#define RADIO_NSS_PORT          GPIOB
#define RADIO_NSS_PIN           LL_GPIO_PIN_12

#define RADIO_DIO_0_PORT        GPIOB
#define RADIO_DIO_0_PIN         LL_GPIO_PIN_4
#define RADIO_DIO_0_EXTI        LL_EXTI_LINE_4
#define RADIO_DIO_0_IRQn        EXTI4_15_IRQn
#define RADIO_DIO_0_CLK         LL_IOP_GRP1_PERIPH_GPIOB

#define RADIO_DIO_1_PORT        GPIOB
#define RADIO_DIO_1_PIN         LL_GPIO_PIN_10
#define RADIO_DIO_1_EXTI        LL_EXTI_LINE_10
#define RADIO_DIO_1_IRQn        EXTI4_15_IRQn
#define RADIO_DIO_1_CLK         LL_IOP_GRP1_PERIPH_GPIOB

#define RADIO_DIO_2_PORT        GPIOA
#define RADIO_DIO_2_PIN         LL_GPIO_PIN_8
#define RADIO_DIO_2_EXTI        LL_EXTI_LINE_8
#define RADIO_DIO_2_IRQn        EXTI4_15_IRQn
#define RADIO_DIO_2_CLK         LL_IOP_GRP1_PERIPH_GPIOA

#define RADIO_DIO_3_PORT        GPIOA
#define RADIO_DIO_3_PIN         LL_GPIO_PIN_11
#define RADIO_DIO_3_EXTI        LL_EXTI_LINE_11
#define RADIO_DIO_3_IRQn        EXTI4_15_IRQn
#define RADIO_DIO_3_CLK         LL_IOP_GRP1_PERIPH_GPIOA

#define RADIO_DIO_4_PORT        GPIOA
#define RADIO_DIO_4_PIN         LL_GPIO_PIN_12
#define RADIO_DIO_4_EXTI        LL_EXTI_LINE_12
#define RADIO_DIO_4_IRQn        EXTI4_15_IRQn
#define RADIO_DIO_4_CLK         LL_IOP_GRP1_PERIPH_GPIOA

#define RADIO_DIO_5_PORT        GPIOB
#define RADIO_DIO_5_PIN         LL_GPIO_PIN_9
#define RADIO_DIO_5_EXTI        LL_EXTI_LINE_9
#define RADIO_DIO_5_IRQn        EXTI4_15_IRQn
#define RADIO_DIO_5_CLK         LL_IOP_GRP1_PERIPH_GPIOB

#define RADIO_ANT_RX_PORT       GPIOB
#define RADIO_ANT_RX_PIN        LL_GPIO_PIN_6
#define RADIO_ANT_TX_PORT       GPIOA
#define RADIO_ANT_TX_PIN        LL_GPIO_PIN_9

/* Public macro --------------------------------------------------------------*/
/* Public functions --------------------------------------------------------- */
void board_error_handler(void);
void board_irq_enable(void);
void board_irq_disable(void);
void board_init(void);
void board_deinit(void);
void board_sleep(void);

void board_delay_ms(uint32_t ms);
void board_get_unique_id(uint8_t *id);
uint32_t board_get_random_seed(void);

#endif /* BOARD_H */
