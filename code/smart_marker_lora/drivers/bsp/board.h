#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <stm32l0xx.h>
#include <stm32l0xx_hal.h>

#include <delay.h>
#include <gpio.h>
#include <radio.h>
#include <rtc-board.h>
#include <spi.h>
#include <sx1272/sx1272.h>
#include <sx1272-board.h>
#include <timer.h>
#include <uart.h>
#include <uart-board.h>
#include <utilities.h>

/*!
 * Generic definition
 */
#ifndef SUCCESS
#define SUCCESS                                     1
#endif

#ifndef FAIL
#define FAIL                                        0
#endif

/*!
 * Board MCU pins definition
 */
#define RADIO_RESET                                 PB_9

#define RADIO_MOSI                                  PA_7
#define RADIO_MISO                                  PA_6
#define RADIO_SCLK                                  PA_5
#define RADIO_NSS                                   PB_12

#define RADIO_DIO_0                                 PB_4
#define RADIO_DIO_1                                 PB_10
#define RADIO_DIO_2                                 PA_8
#define RADIO_DIO_3                                 PA_11
#define RADIO_DIO_4                                 PA_12
#define RADIO_DIO_5                                 PB_8

#define RADIO_ANT_SWITCH_RX                         PB_6
#define RADIO_ANT_SWITCH_TX                         PA_9

#define OSC_LSE_IN                                  PC_14
#define OSC_LSE_OUT                                 PC_15

#define OSC_HSE_IN                                  PH_0
#define OSC_HSE_OUT                                 PH_1

#define SWCLK                                       PA_14
#define SWDAT                                       PA_13

#define UART_TX                                     PA_9
#define UART_RX                                     PA_10

#define BAT_LEVEL_PIN                               PA_0
#define BAT_LEVEL_CHANNEL                           ADC_CHANNEL_0

/*!
 * MCU objects
 */
extern Uart_t Uart2;

/*!
 * Possible power sources
 */
enum BoardPowerSources
{
    USB_POWER = 0,
    BATTERY_POWER,
};

/*!
 * \brief Disable interrupts
 *
 * \remark IRQ nesting is managed
 */
void BoardDisableIrq(void);

/*!
 * \brief Enable interrupts
 *
 * \remark IRQ nesting is managed
 */
void BoardEnableIrq(void);

/*!
 * \brief Initializes the target board peripherals.
 */
void BoardInitMcu(void);

/*!
 * \brief Initializes the boards peripherals.
 */
void BoardInitPeriph(void);

/*!
 * \brief De-initializes the target board peripherals to decrease power
 *        consumption.
 */
void BoardDeInitMcu(void);

/*!
 * \brief Measure the Battery voltage
 *
 * \retval value  battery voltage in volts
 */
uint32_t BoardGetBatteryVoltage(void);

/*!
 * \brief Get the current battery level
 *
 * \retval value  battery level [  0: USB,
 *                                 1: Min level,
 *                                 x: level
 *                               254: fully charged,
 *                               255: Error]
 */
uint8_t BoardGetBatteryLevel(void);

/*!
 * Returns a pseudo random seed generated using the MCU Unique ID
 *
 * \retval seed Generated pseudo random seed
 */
uint32_t BoardGetRandomSeed(void);

/*!
 * \brief Gets the board 64 bits unique ID
 *
 * \param [IN] id Pointer to an array that will contain the Unique ID
 */
void BoardGetUniqueId(uint8_t *id);

/*!
 * \brief Get the board power source
 *
 * \retval value  power source [0: USB_POWER, 1: BATTERY_POWER]
 */
uint8_t GetBoardPowerSource(void);

#endif /* BOARD_H */
