/* Includes ------------------------------------------------------------------*/
#include <stm32l0xx_ll_bus.h>
#include <stm32l0xx_ll_gpio.h>
#include <stm32l0xx_ll_rcc.h>
#include <stm32l0xx_ll_usart.h>

#include <board.h>
#include <uart.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USARTx                          USART1
#define USARTx_CLK_ENABLE()             LL_APB2_GRP1_EnableClock(              \
                                            LL_APB2_GRP1_PERIPH_USART1)
#define USARTx_CLK_SOURCE()             LL_RCC_SetUSARTClockSource(            \
                                            LL_RCC_USART1_CLKSOURCE_PCLK2)
#define USARTx_GPIO_CLK_ENABLE()        LL_IOP_GRP1_EnableClock(               \
                                            LL_IOP_GRP1_PERIPH_GPIOA)

#define USARTx_TX_PIN                   LL_GPIO_PIN_9
#define USARTx_TX_GPIO_PORT             GPIOA
#define USARTx_SET_TX_GPIO_AF()         LL_GPIO_SetAFPin_8_15(GPIOA,           \
                                                              LL_GPIO_PIN_9,   \
                                                              LL_GPIO_AF_4)
#define USARTx_RX_PIN                   LL_GPIO_PIN_10
#define USARTx_RX_GPIO_PORT             GPIOA
#define USARTx_SET_RX_GPIO_AF()         LL_GPIO_SetAFPin_8_15(GPIOA,           \
                                                              LL_GPIO_PIN_10,  \
                                                              LL_GPIO_AF_4)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void
uart_transmit(const uint8_t *data, size_t size)
{
    uint8_t cur_byte = 0;

    /* Send characters one per one, until last char to be sent */
    while (cur_byte < size) {
        /* Wait for TXE flag to be raised */
        while (!LL_USART_IsActiveFlag_TXE(USARTx));

        /* If last char to be sent, clear TC flag */
        if (cur_byte == (size - 1)) {
            LL_USART_ClearFlag_TC(USARTx);
        }

        /* Write character in Transmit Data register.
        TXE flag is cleared by writing data in TDR register */
        LL_USART_TransmitData8(USARTx, data[cur_byte]);
        cur_byte++;
    }

    /* Wait for TC flag to be raised for last char */
    while (!LL_USART_IsActiveFlag_TC(USARTx));
}

static void
uart_receive(uint8_t *data, size_t size)
{
    uint8_t cur_byte = 0;

    while (cur_byte < size) {
        while (!LL_USART_IsActiveFlag_RXNE(USARTx));

        /* If last char to be sent, clear TC flag */
        if (cur_byte == (size - 1)) {
            LL_USART_ClearFlag_TC(USARTx);
        }

        data[cur_byte] = LL_USART_ReceiveData8(USARTx);
        cur_byte++;
    }

    /* Wait for TC flag to be raised for last char */
    while (!LL_USART_IsActiveFlag_TC(USARTx));
}

/* Public functions ----------------------------------------------------------*/
uint8_t
uart_get_char(uint8_t *data)
{
    /* TODO: Add error handling */
    uint8_t tmp = *data;

    uart_receive(data, 1);

    if (tmp != *data) {
        return 1;
    }

    return 0;
}

uint8_t
uart_put_char(uint8_t data)
{
    /* TODO: Add error handling */
    uart_transmit(&data, 1);
    return 0;
}

uint8_t
uart_get_buffer(uint8_t *data, size_t size)
{
    /* TODO: Add error handling */
    uart_receive(data, size);
    return 0;
}

uint8_t
uart_put_buffer(uint8_t *data, size_t size)
{
    /* TODO: Add error handling */
    uart_transmit(data, size);
    return 0;
}

void
uart_init(void)
{
    /* (1) Enable GPIO clock and configures the USART pins ********************/
    /* Enable the peripheral clock of GPIO Port */
    USARTx_GPIO_CLK_ENABLE();

    /* Configure Tx Pin as : AF, High Speed, Push pull, Pull up */
    LL_GPIO_SetPinMode(USARTx_TX_GPIO_PORT,
                       USARTx_TX_PIN,
                       LL_GPIO_MODE_ALTERNATE);
    USARTx_SET_TX_GPIO_AF();
    LL_GPIO_SetPinSpeed(USARTx_TX_GPIO_PORT,
                        USARTx_TX_PIN,
                        LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinOutputType(USARTx_TX_GPIO_PORT,
                             USARTx_TX_PIN,
                             LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinPull(USARTx_TX_GPIO_PORT,
                       USARTx_TX_PIN,
                       LL_GPIO_PULL_UP);

    /* Configure Rx Pin as : AF, High Speed, Push pull, Pull up */
    LL_GPIO_SetPinMode(USARTx_RX_GPIO_PORT,
                       USARTx_RX_PIN,
                       LL_GPIO_MODE_ALTERNATE);
    USARTx_SET_RX_GPIO_AF();
    LL_GPIO_SetPinSpeed(USARTx_RX_GPIO_PORT,
                        USARTx_RX_PIN,
                        LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinOutputType(USARTx_RX_GPIO_PORT,
                             USARTx_RX_PIN,
                             LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinPull(USARTx_RX_GPIO_PORT,
                       USARTx_RX_PIN,
                       LL_GPIO_PULL_UP);

    /* (2) Enable USART peripheral clock and clock source *********************/
    USARTx_CLK_ENABLE();

    /* Set clock source */
    USARTx_CLK_SOURCE();

    /* (3) Configure USART functional parameters ******************************/
    /* Disable USART prior modifying configuration registers */
    /* Note: Commented as corresponding to Reset value */
    // LL_USART_Disable(USARTx);

    /* TX/RX direction */
    LL_USART_SetTransferDirection(USARTx,
                                  LL_USART_DIRECTION_TX_RX);

    /* 8 data bit, 1 start bit, 1 stop bit, no parity */
    LL_USART_ConfigCharacter(USARTx,
                             LL_USART_DATAWIDTH_8B,
                             LL_USART_PARITY_NONE,
                             LL_USART_STOPBITS_1);

    /* No Hardware Flow control */
    /* Reset value is LL_USART_HWCONTROL_NONE */
    // LL_USART_SetHWFlowCtrl(USARTx, LL_USART_HWCONTROL_NONE);

    /* Oversampling by 16 */
    /* Reset value is LL_USART_OVERSAMPLING_16 */
    // LL_USART_SetOverSampling(USARTx, LL_USART_OVERSAMPLING_16);

    /* Set Baudrate to 115200 using APB frequency set to 16000000 Hz */
    /* Frequency available for USART peripheral can also be calculated through
       LL RCC macro */
    /* Ex :
      Periphclk = LL_RCC_GetUSARTClockFreq(Instance);
      or LL_RCC_GetUARTClockFreq(Instance); depending on USART/UART instance

      In this example, Peripheral Clock is expected to be equal to:
      16000000 Hz => equal to SystemCoreClock
    */
    LL_USART_SetBaudRate(USARTx,
                       SystemCoreClock,
                       LL_USART_OVERSAMPLING_16,
                       115200);

    /* (4) Enable USART *******************************************************/
    LL_USART_Enable(USARTx);

    /* Polling USART initialisation */
    while ((!(LL_USART_IsActiveFlag_TEACK(USARTx)))
        || (!(LL_USART_IsActiveFlag_REACK(USARTx))));
}
