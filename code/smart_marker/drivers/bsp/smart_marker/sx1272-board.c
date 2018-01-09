#include <stm32l0xx_ll_bus.h>
#include <stm32l0xx_ll_exti.h>
#include <stm32l0xx_ll_gpio.h>
#include <stm32l0xx_ll_system.h>

#include "board.h"
#include "radio.h"
#include "sx1272/sx1272.h"
#include "sx1272-board.h"

/*!
 * Flag used to set the RF switch control pins in low power mode when the radio is not active.
 */
static bool RadioIsActive = false;

const struct Radio_s Radio = {
    SX1272Init,
    SX1272GetStatus,
    SX1272SetModem,
    SX1272SetChannel,
    SX1272IsChannelFree,
    SX1272Random,
    SX1272SetRxConfig,
    SX1272SetTxConfig,
    SX1272CheckRfFrequency,
    SX1272GetTimeOnAir,
    SX1272Send,
    SX1272SetSleep,
    SX1272SetStby,
    SX1272SetRx,
    SX1272StartCad,
    SX1272SetTxContinuousWave,
    SX1272ReadRssi,
    SX1272Write,
    SX1272Read,
    SX1272WriteBuffer,
    SX1272ReadBuffer,
    SX1272SetMaxPayloadLength,
    SX1272SetPublicNetwork
};

void
SX1272IoInit(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Speed       = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType  = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull        = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate   = LL_GPIO_AF_0;

    // GpioInit(&SX1272.Spi.Nss, RADIO_NSS, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1);
    GPIO_InitStruct.Pin = RADIO_NSS_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    // LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_GPIO_Init(RADIO_NSS_PORT, &GPIO_InitStruct);
    LL_GPIO_SetOutputPin(RADIO_NSS_PORT, RADIO_NSS_PIN);

    // GpioInit(&SX1272.DIO0, RADIO_DIO_0, PIN_INPUT, PIN_PUSH_PULL, PIN_PULL_UP, 0);
    GPIO_InitStruct.Pin = RADIO_DIO_0_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    // LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_GPIO_Init(RADIO_DIO_0_PORT, &GPIO_InitStruct);
    LL_GPIO_ResetOutputPin(RADIO_DIO_0_PORT, RADIO_DIO_0_PIN);

    // GpioInit(&SX1272.DIO1, RADIO_DIO_1, PIN_INPUT, PIN_PUSH_PULL, PIN_PULL_UP, 0);
    GPIO_InitStruct.Pin = RADIO_DIO_1_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    // LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_GPIO_Init(RADIO_DIO_1_PORT, &GPIO_InitStruct);
    LL_GPIO_ResetOutputPin(RADIO_DIO_1_PORT, RADIO_DIO_1_PIN);

    // GpioInit(&SX1272.DIO2, RADIO_DIO_2, PIN_INPUT, PIN_PUSH_PULL, PIN_PULL_UP, 0);
    GPIO_InitStruct.Pin = RADIO_DIO_2_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    // LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_GPIO_Init(RADIO_DIO_2_PORT, &GPIO_InitStruct);
    LL_GPIO_ResetOutputPin(RADIO_DIO_2_PORT, RADIO_DIO_2_PIN);

    // GpioInit(&SX1272.DIO3, RADIO_DIO_3, PIN_INPUT, PIN_PUSH_PULL, PIN_PULL_UP, 0);
    GPIO_InitStruct.Pin = RADIO_DIO_3_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    // LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_GPIO_Init(RADIO_DIO_3_PORT, &GPIO_InitStruct);
    LL_GPIO_ResetOutputPin(RADIO_DIO_3_PORT, RADIO_DIO_3_PIN);

    // DIO4 and DIO5 aren't connected.
    // SX1272.DIO4.port = NULL;
    // SX1272.DIO5.port = NULL;
}

void
SX1272IoIrqInit(DioIrqHandler **irqHandlers)
{
    LL_EXTI_InitTypeDef EXTI_InitStruct;

    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode        = LL_EXTI_MODE_IT;
    EXTI_InitStruct.Trigger     = LL_EXTI_TRIGGER_RISING;

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);

    // GpioSetInterrupt(&SX1272.DIO0, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, irqHandlers[0]);
    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTC, LL_SYSCFG_EXTI_LINE13);

    EXTI_InitStruct.Line_0_31   = RADIO_DIO_0_EXTI;
    LL_EXTI_Init(&EXTI_InitStruct);
    NVIC_EnableIRQ(RADIO_DIO_0_IRQn);
    NVIC_SetPriority(RADIO_DIO_0_IRQn, 0);

    // GpioSetInterrupt(&SX1272.DIO1, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, irqHandlers[1]);
    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTC, LL_SYSCFG_EXTI_LINE13);

    EXTI_InitStruct.Line_0_31   = RADIO_DIO_1_EXTI;
    LL_EXTI_Init(&EXTI_InitStruct);
    NVIC_EnableIRQ(RADIO_DIO_1_IRQn);
    NVIC_SetPriority(RADIO_DIO_1_IRQn, 0);

    // GpioSetInterrupt(&SX1272.DIO2, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, irqHandlers[2]);
    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTC, LL_SYSCFG_EXTI_LINE13);

    EXTI_InitStruct.Line_0_31   = RADIO_DIO_2_EXTI;
    LL_EXTI_Init(&EXTI_InitStruct);
    NVIC_EnableIRQ(RADIO_DIO_2_IRQn);
    NVIC_SetPriority(RADIO_DIO_2_IRQn, 0);

    // GpioSetInterrupt(&SX1272.DIO3, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, irqHandlers[3]);
    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTC, LL_SYSCFG_EXTI_LINE13);

    EXTI_InitStruct.Line_0_31   = RADIO_DIO_3_EXTI;
    LL_EXTI_Init(&EXTI_InitStruct);
    NVIC_EnableIRQ(RADIO_DIO_3_IRQn);
    NVIC_SetPriority(RADIO_DIO_3_IRQn, 0);
}

void
SX1272IoDeInit(void)
{
    /* TODO: fix*/
    // GpioInit(&SX1272.Spi.Nss, RADIO_NSS, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1);

    // GpioInit(&SX1272.DIO0, RADIO_DIO_0, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    // GpioInit(&SX1272.DIO1, RADIO_DIO_1, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    // GpioInit(&SX1272.DIO2, RADIO_DIO_2, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    // GpioInit(&SX1272.DIO3, RADIO_DIO_3, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
}

void
SX1272SetRfTxPower(int8_t power)
{
    uint8_t paConfig = 0;
    uint8_t paDac = 0;

    paConfig = SX1272Read(REG_PACONFIG);
    paDac = SX1272Read(REG_PADAC);

    paConfig = (paConfig & RF_PACONFIG_PASELECT_MASK)
               | SX1272GetPaSelect(SX1272.Settings.Channel);

    if ((paConfig & RF_PACONFIG_PASELECT_PABOOST)
        == RF_PACONFIG_PASELECT_PABOOST) {
        if (power > 17) {
            paDac = (paDac & RF_PADAC_20DBM_MASK) | RF_PADAC_20DBM_ON;
        } else {
            paDac = (paDac & RF_PADAC_20DBM_MASK) | RF_PADAC_20DBM_OFF;
        }

        if ((paDac & RF_PADAC_20DBM_ON) == RF_PADAC_20DBM_ON) {
            if (power < 5) {
                power = 5;
            }

            if (power > 20) {
                power = 20;
            }

            paConfig = (paConfig & RFLR_PACONFIG_OUTPUTPOWER_MASK)
                       | (uint8_t)((uint16_t)(power - 5) & 0x0F);
        } else {
            if (power < 2) {
                power = 2;
            }

            if (power > 17) {
                power = 17;
            }

            paConfig = (paConfig & RFLR_PACONFIG_OUTPUTPOWER_MASK)
                       | (uint8_t)((uint16_t)(power - 2) & 0x0F);
        }
    } else {
        if (power < -1) {
            power = -1;
        }

        if (power > 14) {
            power = 14;
        }

        paConfig = (paConfig & RFLR_PACONFIG_OUTPUTPOWER_MASK)
                   | (uint8_t)((uint16_t)(power + 1) & 0x0F);
    }

    SX1272Write(REG_PACONFIG, paConfig);
    SX1272Write(REG_PADAC,    paDac);
}

uint8_t
SX1272GetPaSelect(uint32_t channel)
{
    // return RF_PACONFIG_PASELECT_RFO;
    return RF_PACONFIG_PASELECT_PABOOST;
}

void
SX1272SetAntSwLowPower(bool status)
{
    if (RadioIsActive != status) {
        RadioIsActive = status;

        if (status == false) {
            SX1272AntSwInit();
        } else {
            SX1272AntSwDeInit();
        }
    }
}

void
SX1272AntSwInit(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Speed       = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType  = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull        = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate   = LL_GPIO_AF_0;

    // GpioInit(&AntTx, RADIO_ANT_SWITCH_TX, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 0);
    GPIO_InitStruct.Pin = RADIO_ANT_TX_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    // LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_GPIO_Init(RADIO_ANT_TX_PORT, &GPIO_InitStruct);
    LL_GPIO_ResetOutputPin(RADIO_ANT_TX_PORT, RADIO_ANT_TX_PIN);

    // GpioInit(&AntRx, RADIO_ANT_SWITCH_RX, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1);
    GPIO_InitStruct.Pin = RADIO_ANT_RX_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    // LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_GPIO_Init(RADIO_ANT_RX_PORT, &GPIO_InitStruct);
    LL_GPIO_SetOutputPin(RADIO_ANT_RX_PORT, RADIO_ANT_RX_PIN);
}

void
SX1272AntSwDeInit(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Speed       = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType  = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull        = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate   = LL_GPIO_AF_0;

    // GpioInit(&AntTx, RADIO_ANT_SWITCH_TX, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    GPIO_InitStruct.Pin = RADIO_ANT_TX_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
    // LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_GPIO_Init(RADIO_ANT_TX_PORT, &GPIO_InitStruct);

    // GpioInit(&AntRx, RADIO_ANT_SWITCH_RX, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0);
    GPIO_InitStruct.Pin = RADIO_ANT_RX_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    // LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_GPIO_Init(RADIO_ANT_RX_PORT, &GPIO_InitStruct);
}

void
SX1272SetAntSw(uint8_t opMode)
{
    switch(opMode)
    {
    case RFLR_OPMODE_TRANSMITTER:
        LL_GPIO_ResetOutputPin(RADIO_ANT_TX_PORT, RADIO_ANT_TX_PIN);
        LL_GPIO_SetOutputPin(RADIO_ANT_RX_PORT, RADIO_ANT_RX_PIN);
        break;
    case RFLR_OPMODE_RECEIVER:
    case RFLR_OPMODE_RECEIVER_SINGLE:
    case RFLR_OPMODE_CAD:
    default:
        LL_GPIO_ResetOutputPin(RADIO_ANT_RX_PORT, RADIO_ANT_RX_PIN);
        LL_GPIO_SetOutputPin(RADIO_ANT_TX_PORT, RADIO_ANT_TX_PIN);
        break;
    }
}

bool
SX1272CheckRfFrequency(uint32_t frequency)
{
    // Implement check. Currently all frequencies are supported
    return true;
}
