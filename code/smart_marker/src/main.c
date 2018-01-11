/* Includes ------------------------------------------------------------------*/
#include <board.h>
#include <lora.h>
#include <definitions.h>
#include <ublox.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SLAVE_ADDRESS (0x42 << 1)
#define MARKER_ID 0

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
int
main(void)
{
    uint8_t output[50];

    board_init();
    lora_init();

    set_message(SLAVE_ADDRESS, GGA, 0);
    set_message(SLAVE_ADDRESS, GNS, 2);
    set_message(SLAVE_ADDRESS, RMC, 0);
    set_message(SLAVE_ADDRESS, GLL, 0);
    set_message(SLAVE_ADDRESS, VTG, 0);

    while (1) {
       i2c_receive_nmea(SLAVE_ADDRESS);
       parse_data(MARKER_ID, output);

        if (lora_network_is_joined()) {
            lora_prepare_frame(output, sizeof(output));
            lora_send_frame();
        } else {
            lora_join();
        }

        // board_sleep();
    }
}

