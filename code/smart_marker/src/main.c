/* Includes ------------------------------------------------------------------*/
#include <board.h>
#include <lora.h>
#include <definitions.h>
#include <ublox.h>

#include <timer.h>
#include <rtc.h>
#include <stm32l0xx_ll_rtc.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SLAVE_ADDRESS (0x42 << 1)
#define MARKER_ID 0

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static TimerEvent_t test_timer;

static uint32_t counter = 0;

/* Private functions ---------------------------------------------------------*/
static void
test_timer_handler(void)
{
    counter++;

    TimerStop(&test_timer);
    TimerStart(&test_timer);
}


int
main(void)
{
    uint8_t test_buf[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x00, 0xCA, 0xFE, 0xBA, 0xBE };
    uint8_t output[50];

    board_init();
    lora_init();

    // TimerInit(&test_timer, test_timer_handler);
    // TimerSetValue(&test_timer, 2000);
    // TimerStart(&test_timer);

    // set_message(SLAVE_ADDRESS, GGA, 0);
    // set_message(SLAVE_ADDRESS, GNS, 2);
    // set_message(SLAVE_ADDRESS, RMC, 0);
    // set_message(SLAVE_ADDRESS, GLL, 0);
    // set_message(SLAVE_ADDRESS, VTG, 0);

    while (1) {
    //    i2c_receive_nmea(SLAVE_ADDRESS);
    //    parse_data(MARKER_ID, output);

        if (lora_network_is_joined()) {
            lora_prepare_frame(test_buf, sizeof(test_buf));
            lora_send_frame();
        } else {
            lora_join();
        }

        // board_sleep();
    }
}

