/* Includes ------------------------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>

#include <board.h>
#include <gps.h>
#include <i2c.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define GPS_I2C_SLAVE_ADDR      (0x42 << 1)

#define GPS_BUF_SIZE_MAX        10

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
void
gps_init(struct gps_data *gps_data)
{
    gps_data->buf = NULL;
    gps_data->size = 0;
}

void
gps_start(void)
{
    i2c_set_receive_address(GPS_I2C_SLAVE_ADDR, 0xFF);
    i2c_receive_string(GPS_I2C_SLAVE_ADDR);
}

void
gps_stop(void)
{

}

void
gps_read(struct gps_data *gps_data)
{
    uint8_t test_buf[GPS_BUF_SIZE_MAX] = {
        0xBE, 0xEF, 0xCA, 0xCE, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
    };

    gps_data->buf = test_buf;
    gps_data->size = GPS_BUF_SIZE_MAX;
}
