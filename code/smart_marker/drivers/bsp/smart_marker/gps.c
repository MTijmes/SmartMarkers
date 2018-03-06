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
}

void
gps_start(void)
{
}

void
gps_stop(void)
{
}

void
gps_read(struct gps_data *gps_data)
{
}
