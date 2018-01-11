/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>

#include <stm32l0xx_ll_bus.h>
#include <stm32l0xx_ll_gpio.h>
#include <stm32l0xx_ll_spi.h>

#include <board.h>
#include <spi.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static bool spi_initialized;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void
spi_gpio_init(void)
{
    LL_GPIO_InitTypeDef gpio_init;

    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);

    gpio_init.Speed       = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init.OutputType  = LL_GPIO_OUTPUT_PUSHPULL;
    gpio_init.Mode        = LL_GPIO_MODE_OUTPUT;
    gpio_init.Pull        = LL_GPIO_PULL_UP;

    gpio_init.Pin = RADIO_NSS_PIN;
    LL_GPIO_Init(RADIO_NSS_PORT, &gpio_init);
    LL_GPIO_SetOutputPin(RADIO_NSS_PORT, RADIO_NSS_PIN);

    gpio_init.Mode        = LL_GPIO_MODE_ALTERNATE;
    gpio_init.Alternate   = LL_GPIO_AF_0;
    gpio_init.Pull        = LL_GPIO_PULL_DOWN;

    gpio_init.Pin = RADIO_MOSI_PIN;
    LL_GPIO_Init(RADIO_MOSI_PORT, &gpio_init);
    LL_GPIO_ResetOutputPin(RADIO_MOSI_PORT, RADIO_MOSI_PIN);

    gpio_init.Pin = RADIO_MISO_PIN;
    LL_GPIO_Init(RADIO_MISO_PORT, &gpio_init);
    LL_GPIO_ResetOutputPin(RADIO_MISO_PORT, RADIO_MISO_PIN);

    gpio_init.Pin = RADIO_SCLK_PIN;
    LL_GPIO_Init(RADIO_SCLK_PORT, &gpio_init);
    LL_GPIO_ResetOutputPin(RADIO_MOSI_PORT, RADIO_SCLK_PIN);
}

static void
spi_periph_init(void)
{
    LL_SPI_InitTypeDef spi_init;

    LL_SPI_DeInit(SPI1);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

    spi_init.BaudRate          = LL_SPI_BAUDRATEPRESCALER_DIV32;
    spi_init.TransferDirection = LL_SPI_FULL_DUPLEX;
    spi_init.ClockPhase        = LL_SPI_PHASE_1EDGE;
    spi_init.ClockPolarity     = LL_SPI_POLARITY_LOW;
    spi_init.BitOrder          = LL_SPI_MSB_FIRST;
    spi_init.DataWidth         = LL_SPI_DATAWIDTH_8BIT;
    spi_init.NSS               = LL_SPI_NSS_SOFT;
    spi_init.CRCCalculation    = LL_SPI_CRCCALCULATION_DISABLE;
    spi_init.Mode              = LL_SPI_MODE_MASTER;
    LL_SPI_Init(SPI1, &spi_init);
}

static uint8_t
spi_in_out(uint8_t tx_byte)
{
    uint8_t rx_byte;

    board_irq_disable();

    while (LL_SPI_IsActiveFlag_TXE(SPI1) == 0);
    LL_SPI_TransmitData8(SPI1, tx_byte);

    while (LL_SPI_IsActiveFlag_RXNE(SPI1) == 0);
    rx_byte = LL_SPI_ReceiveData8(SPI1);

    board_irq_enable();

    return rx_byte;
}

/* Public functions ----------------------------------------------------------*/
void
spi_init(void)
{
    if (!spi_initialized) {
        spi_gpio_init();
        spi_periph_init();

        LL_SPI_Enable(SPI1);

        spi_initialized = true;
    }
}

void
spi_write_buf(uint8_t addr, uint8_t *buf, size_t size)
{
    uint8_t i;

    LL_GPIO_ResetOutputPin(RADIO_NSS_PORT, RADIO_NSS_PIN);

    spi_in_out(addr | 0x80);
    for (i = 0; i < size; i++) {
        spi_in_out(buf[i]);
    }

    LL_GPIO_SetOutputPin(RADIO_NSS_PORT, RADIO_NSS_PIN);
}

void
spi_read_buf(uint8_t addr, uint8_t *buf, size_t size)
{
    uint8_t i;

    LL_GPIO_ResetOutputPin(RADIO_NSS_PORT, RADIO_NSS_PIN);

    spi_in_out(addr & 0x7F);
    for (i = 0; i < size; i++) {
        buf[i] = spi_in_out(0);
    }

    LL_GPIO_SetOutputPin(RADIO_NSS_PORT, RADIO_NSS_PIN);
}
