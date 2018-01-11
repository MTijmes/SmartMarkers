/* Includes ------------------------------------------------------------------*/
#include <board.h>
#include <gps.h>
#include <lora.h>

#include <uart.h>
#include <string.h>
#include <stdio.h>
#include <definitions.h>
#include <ublox.h>
#include <timer.h>
#include <rtc.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SLAVE_ADDRESS (0x42 << 1)
#define MARKER_ID 0

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t output[80];

/* Private functions ---------------------------------------------------------*/
int
main(void)
{
    // struct gps_data gps_data;

    board_init();
    // gps_init(&gps_data);
    lora_init();

    //set_message(SLAVE_ADDRESS, GGA, 0);
    //set_message(SLAVE_ADDRESS, GNS, 2);
    //set_message(SLAVE_ADDRESS, RMC, 0);
    //set_message(SLAVE_ADDRESS, GLL, 0);
    //set_message(SLAVE_ADDRESS, VTG, 0);
    TimerTime_t timep = 0;

    while (1) {
       // i2c_receive_nmea(SLAVE_ADDRESS);
       // parse_data(MARKER_ID, output);

        // if (lora_network_is_joined()) {
        //     // lora_prepare_frame(gps_data.buf, gps_data.size);
        //     lora_send_frame();
        // } else {
        //     lora_join();
        // }
        timep = RtcGetTimerValue();

        board_sleep();
    }
}

