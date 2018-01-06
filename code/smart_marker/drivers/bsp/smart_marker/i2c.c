// Includes --------------------------------------------------------------------
#include "i2c.h"
#include <stdint.h>

// Defines ---------------------------------------------------------------------
// Timing register: @400kHz, 32Mhz clock, rise time = 100ns, fall time = 10ns
#define I2C_TIMING 0x00601135

// Private variables -----------------------------------------------------------
__IO uint8_t receiveIndex = 0;
uint8_t *receiveBuffer;
uint8_t size;

/**
 * @brief  This function configures I2C2 in Master mode.
 * @note   This function is used to :
 *         -1- Enables GPIO clock and configures the I2C2 pins.
 *         -2- Enable the I2C2 peripheral clock and I2C2 clock source.
 *         -3- Configure NVIC for I2C2.
 *         -4- Configure I2C2 functional parameters.
 *         -5- Enable I2C2.
 *         -6- Enable I2C2 transfer complete/error interrupts.
 * @note   Peripheral configuration is minimal configuration from reset values.
 *         Thus, some useless LL unitary functions calls below are provided as
 *         commented examples - setting is default configuration from reset.
 * @param  None
 * @retval None
 */
void
i2c_init(uint8_t *buf, uint8_t buffersize)
{
    // Enable the peripheral clock of GPIOC
    LL_I2C_InitTypeDef I2C_InitStruct;
    LL_GPIO_InitTypeDef GPIO_InitStruct;

    /* I2C2 GPIO Configuration
       PB13   ------> I2C2_SCL
       PB14   ------> I2C2_SDA */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_13;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_14;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Peripheral clock enable
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);

    // I2C init
    I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
    I2C_InitStruct.Timing = 0x00601B28;
    I2C_InitStruct.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
    I2C_InitStruct.DigitalFilter = 0;
    I2C_InitStruct.OwnAddress1 = 0;
    I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
    I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;

    LL_I2C_Init(I2C2, &I2C_InitStruct);
    LL_I2C_EnableAutoEndMode(I2C2);
    LL_I2C_SetOwnAddress2(I2C2, 0, LL_I2C_OWNADDRESS2_NOMASK);
    LL_I2C_DisableOwnAddress2(I2C2);
    LL_I2C_DisableGeneralCall(I2C2);
    LL_I2C_EnableClockStretching(I2C2);

    receiveBuffer= buf;
    size = buffersize;
}

void
i2c_set_receive_address(uint32_t slave_address, uint8_t address)
{
    // Request write to i2c device
    LL_I2C_HandleTransfer(I2C2,
                          slave_address,
                          LL_I2C_ADDRSLAVE_7BIT,
                          1,
                          LL_I2C_MODE_AUTOEND,
                          LL_I2C_GENERATE_START_WRITE);
    // Transmit address to i2c device
    LL_I2C_TransmitData8(I2C2, address);
}

void
i2c_receive_string(uint32_t slave_address)
{
    uint8_t temp = 0;
    receiveIndex = 0;
    while (temp != '\n') {
        while (LL_I2C_IsActiveFlag_RXNE(I2C2)) {
            temp = LL_I2C_ReceiveData8(I2C2);
            if (temp != 255) {
                receiveBuffer[receiveIndex++] = temp;
                if (receiveIndex > size) {
                    receiveIndex = 0;
                }
            }
        }
        if (LL_I2C_IsActiveFlag_STOP(I2C2)) {
            LL_I2C_ClearFlag_STOP(I2C2);
            i2c_continue_receiving(slave_address);
        }
    }
    LL_I2C_ClearFlag_STOP(I2C2);
}

void
i2c_continue_receiving(uint32_t slave_address)
{
    // Ask for a new piece of data
    LL_I2C_HandleTransfer(I2C2,
                          slave_address,
                          LL_I2C_ADDRSLAVE_7BIT,
                          1,
                          LL_I2C_MODE_AUTOEND,
                          LL_I2C_GENERATE_START_READ);
}

void
i2c_error_callback(void)
{
    // Disable I2C2_IRQn
    NVIC_DisableIRQ(I2C2_IRQn);
}

void
i2c_start_write(uint32_t slave_address,uint8_t numbytes)
{
    LL_I2C_HandleTransfer(I2C2,
                          slave_address,
                          LL_I2C_ADDRSLAVE_7BIT,
                          numbytes,
                          LL_I2C_MODE_AUTOEND,
                          LL_I2C_GENERATE_START_WRITE);
}

void
i2c_write(uint32_t slave_address, uint8_t *message, uint8_t size)
{
    int i = 0;
    LL_I2C_ClearFlag_STOP(I2C2);
    while(i < size) {
        while(!LL_I2C_IsActiveFlag_STOP(I2C2)) {
            if(LL_I2C_IsActiveFlag_TXIS(I2C2)) {
                LL_I2C_TransmitData8(I2C2, message[i]);
                i++;
            } else if(LL_I2C_IsActiveFlag_TC(I2C2)) {
                LL_I2C_GenerateStopCondition(I2C2);
            }
        }
        int i = 0;
        i2c_start_write(slave_address, size);
    }
    LL_I2C_GenerateStopCondition(I2C2);
}

void
assert_failed(uint8_t *file, uint32_t line)
{}