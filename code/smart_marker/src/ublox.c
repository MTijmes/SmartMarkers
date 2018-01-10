/* Includes ------------------------------------------------------------------*/
#include <stm32l0xx_ll_i2c.h>

#include <ublox.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
char strdelim[80][12];

/* Global variables ----------------------------------------------------------*/
extern uint8_t i2c_receive_buffer[];

/* Private function prototypes -----------------------------------------------*/
char* strtok_single(char *str, char const *delims);
void set_rate(uint32_t slave_address, uint8_t *payload);
uint8_t cfg_ack_receive(uint32_t slave_address);
void fill_ubx_message(uint8_t *ubx_buffer,
                      char    class_id,
                      char    message_id,
                      uint8_t length_of_payload,
                      uint8_t *payload);
void delimit_string(char *string);

/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
void
set_rate(uint32_t slave_address, uint8_t *payload)
{
    uint8_t ubx_message[32];
    fill_ubx_message(ubx_message, 0x06, 0x01, 8, payload);
    // Ublox likes to send 255 + ACK from time to time so we set transmit to 17
    i2c_start_write(slave_address, 17);
    i2c_write(slave_address, ubx_message, 16);
    // Ask for a new piece of data
    LL_I2C_ClearFlag_STOP(I2C2);
    while(LL_I2C_IsActiveFlag_BUSY(I2C2)) {} // Make sure we can write
    if(cfg_ack_receive(slave_address)) { // Check for ACK
        return;
    } else {
        set_rate(slave_address, payload); // Try again
    }
    while(LL_I2C_IsActiveFlag_BUSY(I2C2)) {}
}

//splits a delimited string into tokens
static char *
strtok_single(char *str, char const *delims)
{
    static char *src = NULL;
    char *p, *ret = 0;

    if(str != NULL)
        src = str;
    //done
    if(src == NULL)
        return NULL;

    if((p = strpbrk (src, delims)) != NULL) {
        *p  = 0;
        ret = src;
        src = ++p;

    } else if(*src) {
        ret = src;
        src = NULL;
    }
    return ret;
}

static void
delimit_string(char *string)
{
    char *token;
    uint8_t count = 0;
    token = strtok_single((char *)string, ",");
    while(token != NULL) {
        strcpy(strdelim[count], token);
        count++;
        token = strtok_single(NULL, ",");
    }
}

/* Public functions ----------------------------------------------------------*/
void
i2c_receive_nmea(uint32_t slave_address)
{
    uint8_t temp = 0;
    uint8_t flag = 0;
    receiveIndex = 1;
    i2c_set_receive_address(slave_address, 0xFF);
    while (temp != '\n') {
        while (LL_I2C_IsActiveFlag_RXNE(I2C2)) {
            temp = LL_I2C_ReceiveData8(I2C2);
            if(temp == '$') { //Wait for actual nmea string to start
                flag = 1;
            } else if(temp == 'G' && flag == 1) {
                flag = 2;
                i2c_receive_buffer[0] = '$';
            }
            if (temp != 255) {
                if(flag == 2) {
                    i2c_receive_buffer[receiveIndex++] = temp;
                }
                if (receiveIndex > I2C_RECEIVE_SIZE) {
                    receiveIndex = 0;
                }
            }
        }
        if (LL_I2C_IsActiveFlag_STOP(I2C2)) {
            LL_I2C_ClearFlag_STOP(I2C2);
            i2c_continue_receiving(slave_address);
        }
    }
    LL_I2C_ClearFlag_STOP(I2C2);
}

void
parse_data(uint8_t marker_id, uint8_t *output)
{
    delimit_string((char *)i2c_receive_buffer);
    //sometimes it does the "$" double while reading i2c
    if(!strcmp(strdelim[0], "$GPGSA")) {
        //third variable in $GPGSA string is fix data, '3' is 3D-fix
        if(!strcmp(strdelim[2], "3")) {}
    } else if(!strcmp(strdelim[0],
                      "$GPGNS")) {
        //markerid, utctime, latitude, NZ, longtitude, EW, altitude
        sprintf((char *)output,
                "%u,%s,%s,%s,%s,%s,%s",
                marker_id,
                strdelim[1],
                strdelim[2],
                strdelim[3],
                strdelim[4],
                strdelim[5],
                strdelim[9]);
    }
    #ifdef REFERENCE
    else if(!strcmp(strdelim[0],
                    "$GPGBS")) {
        sprintf((char *)output,
                "%s,%s,%s,%s",
                output,
                strdelim[2],
                strdelim[3],
                strdelim[4])
    }
    #else
    #endif
}

void
set_message(uint32_t slave_adress, uint8_t message_id, uint8_t rate)
{
    uint8_t payload[8];
    payload[0] = NMEA;          //NMEA class id
    payload[1] = message_id;    //message id
    payload[2] = rate;             //I2C
    payload[3] = 0;             //UART1
    payload[4] = 0;             //USB
    payload[5] = 0;             //SPI
    payload[6] = 0;             //Reserved I/O
    payload[7] = 0;             //Reserved I/O
    set_rate(slave_adress, payload);
}

void
fill_ubx_message(uint8_t *ubx_buffer,
                 char    class_id,
                 char    message_id,
                 uint8_t length_of_payload,
                 uint8_t *payload)
{
    uint8_t ck_a, ck_b, n;
    n = 6 + length_of_payload;
    ck_a = 0;
    ck_b = 0;
    ubx_buffer[0] = SYNC1;
    ubx_buffer[1] = SYNC2;
    ubx_buffer[2] = class_id;
    ubx_buffer[3] = message_id;
    ubx_buffer[4] = length_of_payload;
    ubx_buffer[5] = 0;
    for(int i=0; i<length_of_payload; i++)
    {
        ubx_buffer[i+6] = payload[i];
    }
    for(int i=2; i<n; i++)
    {
        ck_a += ubx_buffer[i];
        ck_b += ck_a;
    }
    ck_a = (ck_a & 0xff);
    ck_b = (ck_b & 0xff);
    ubx_buffer[6+length_of_payload] = ck_a;
    ubx_buffer[7+length_of_payload] = ck_b;
}
