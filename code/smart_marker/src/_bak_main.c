/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TRANSMITTER_BOARD

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* UART handler declaration */
__IO ITStatus UartReady = RESET;
__IO uint32_t UserButtonStatus = 0;  /* set to 1 after User Button interrupt  */

/* Buffer used for transmission */
uint8_t aTxBuffer[] = " ****UART_TwoBoards_ComIT**** ";

/* Buffer used for reception */
uint8_t aRxBuffer[RXBUFFERSIZE];

/* Private function prototypes -----------------------------------------------*/
static uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);

/* Private functions ---------------------------------------------------------*/
int main(void)
{
    board_init();

    // lora_init();
    // gps_init();

    BSP_LED_Init(LED2);

#ifdef TRANSMITTER_BOARD

    /* Configure User push-button in Interrupt mode */
    BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);

    /* Wait for User push-button press before starting the Communication.
    In the meantime, LED2 is blinking */
    while(UserButtonStatus == 0) {
        /* Toggle LED2*/
        BSP_LED_Toggle(LED2);
        HAL_Delay(100);
    }

    BSP_LED_Off(LED2);


    /* The board sends the message and expects to receive it back */

    /*##-2- Start the transmission process #####################################*/
    /* While the UART in reception process, user can transmit data through
    "aTxBuffer" buffer */
    if(HAL_UART_Transmit_IT(&UartHandle, (uint8_t*)aTxBuffer, TXBUFFERSIZE)!= HAL_OK) {
        Error_Handler();
    }

    /*##-3- Wait for the end of the transfer ###################################*/
    while (UartReady != SET);

    /* Reset transmission flag */
    UartReady = RESET;

    /*##-4- Put UART peripheral in reception process ###########################*/
    if(HAL_UART_Receive_IT(&UartHandle, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK) {
        Error_Handler();
    }

#else

    /* The board receives the message and sends it back */

    /*##-2- Put UART peripheral in reception process ###########################*/
    if(HAL_UART_Receive_IT(&UartHandle, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK) {
        Error_Handler();
    }

    /*##-3- Wait for the end of the transfer ###################################*/
    /* While waiting for message to come from the other board, LED2 is
    blinking according to the following pattern: a double flash every half-second */
    while (UartReady != SET) {
        BSP_LED_On(LED2);
        HAL_Delay(100);
        BSP_LED_Off(LED2);
        HAL_Delay(100);
        BSP_LED_On(LED2);
        HAL_Delay(100);
        BSP_LED_Off(LED2);
        HAL_Delay(500);
    }

    /* Reset transmission flag */
    UartReady = RESET;
    BSP_LED_Off(LED2);

    /*##-4- Start the transmission process #####################################*/
    /* While the UART in reception process, user can transmit data through
    "aTxBuffer" buffer */
    if(HAL_UART_Transmit_IT(&UartHandle, (uint8_t*)aTxBuffer, TXBUFFERSIZE)!= HAL_OK) {
        Error_Handler();
    }

#endif /* TRANSMITTER_BOARD */

    /*##-5- Wait for the end of the transfer ###################################*/
    while (UartReady != SET);

    /* Reset transmission flag */
    UartReady = RESET;

    /*##-6- Compare the sent and received buffers ##############################*/
    if(Buffercmp((uint8_t*)aTxBuffer, (uint8_t*)aRxBuffer, RXBUFFERSIZE)) {
        Error_Handler();
    }

    /* Turn on LED2 if test passes then enter infinite loop */
    BSP_LED_On(LED2);

    /* Infinite loop */
    while (1);
}

/**
* @brief  Tx Transfer completed callback
* @param  UartHandle: UART handle.
* @note   This example shows a simple way to report end of IT Tx transfer, and
*         you can add your own implementation.
* @retval None
*/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
    /* Set transmission flag: transfer complete */
    UartReady = SET;
}

/**
* @brief  Rx Transfer completed callback
* @param  UartHandle: UART handle
* @note   This example shows a simple way to report end of DMA Rx transfer, and
*         you can add your own implementation.
* @retval None
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
    /* Set transmission flag: transfer complete */
    UartReady = SET;
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
    Error_Handler();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == KEY_BUTTON_PIN) {
        UserButtonStatus = 1;
    }
}

/**
* @brief  Compares two buffers.
* @param  pBuffer1, pBuffer2: buffers to be compared.
* @param  BufferLength: buffer's length
* @retval 0  : pBuffer1 identical to pBuffer2
*         >0 : pBuffer1 differs from pBuffer2
*/
static uint16_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
    while (BufferLength--) {
        if ((*pBuffer1) != *pBuffer2) {
            return BufferLength;
        }

        pBuffer1++;
        pBuffer2++;
    }

    return 0;
}
