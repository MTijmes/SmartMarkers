/* Includes ------------------------------------------------------------------*/
#include <board.h>
#include <lora.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
int
main(void)
{
    uint8_t test_buf[] = { 0xBE, 0xEF, 0xCA, 0xCE, 0x00, 0x11, 0x22, 0x33 };

    board_init();
    lora_init();

    while (1) {
        if (lora_network_is_joined()) {
            lora_prepare_frame(test_buf, sizeof(test_buf));
            lora_send_frame();
        } else {
            lora_join();
        }

        // board_sleep();
    }
}

