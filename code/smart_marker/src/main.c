/* Includes ------------------------------------------------------------------*/
#include <board.h>
#include <uart.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
int main(void)
{
    board_init();

    // lora_init();
    // gps_init();

    uint8_t test_byte = 0;
    uint8_t  = 0;

    while (1) {
        // uart_put_buffer("Hello, world!\r\n", 16);
        // uart_put_char('c');
        if (uart_get_char(&test_byte)) {
            uart_put_char(test_byte);
        }
    }
}
