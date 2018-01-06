/* Includes ------------------------------------------------------------------*/
#include <board.h>
#include <uart.h>
#include <i2c.h>
#include <string.h>
#include <stdio.h>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SLAVE_ADDRESS 0x42 << 1
#define MARKER_ID 0
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t readytosend;
uint8_t output[80];
char strdelim[80][12];
/* Private function prototypes -----------------------------------------------*/
void  delimit_string(char *string);
char*strtok_single (char *str, char const *delims);
void parse_data(uint8_t *output);
void fill_ubx_message(char    *ubx_buffer,
                      char    class_id,
                      char    message_id,
                      uint8_t length_of_payload,
                      char    *payload);
void set_rate(char *ubx_message, char *payload);
/* Private functions ---------------------------------------------------------*/
int
main(void)
{
    board_init();

    // lora_init();
    // gps_init();

    uint8_t ubx_message[32];
    uint8_t something = 0;
    uint8_t i = 0;
    uint8_t payload[8];
    payload[0] = 0xF0;          //NMEA class id
    payload[1] = 0x00;          //GGA message id
    payload[2] = 0;             //I2C
    payload[3] = 0;             //UART1
    payload[4] = 0;             //USB
    payload[5] = 0;             //SPI
    payload[6] = 0;             //Reserved I/O
    payload[7] = 0;             //Reserved I/O
    set_rate(ubx_message,&payload);

    payload[0] = 0xF0;              //NMEA class id
    payload[1] = 0x0D;              //GNS message id
    payload[2] = 2;                 //I2C
    payload[3] = 0;                 //UART1
    payload[4] = 0;             //USB
    payload[5] = 0;             //SPI
    payload[6] = 0;             //Reserved I/O
    payload[7] = 0;             //Reserved I/O
    set_rate(ubx_message,&payload);

    payload[0] = 0xF0;                  //NMEA class id
    payload[1] = 0x04;                  //RMC message id
    payload[2] = 0;                     //I2C
    payload[3] = 0;                 //UART1
    payload[4] = 0;             //USB
    payload[5] = 0;             //SPI
    payload[6] = 0;             //Reserved I/O
    payload[7] = 0;             //Reserved I/O
    set_rate(ubx_message,&payload);

    payload[0] = 0xF0;                  //NMEA class id
    payload[1] = 0x01;                  //GLL message id
    payload[2] = 0;                     //I2C
    payload[3] = 0;                 //UART1
    payload[4] = 0;             //USB
    payload[5] = 0;             //SPI
    payload[6] = 0;             //Reserved I/O
    payload[7] = 0;             //Reserved I/O
    set_rate(ubx_message,&payload);

    payload[0] = 0xF0;                  //NMEA class id
    payload[1] = 0x05;                  //VTG message id
    payload[2] = 0;                     //I2C
    payload[3] = 0;                 //UART1
    payload[4] = 0;             //USB
    payload[5] = 0;             //SPI
    payload[6] = 0;             //Reserved I/O
    payload[7] = 0;             //Reserved I/O
    set_rate(ubx_message,&payload);

    while (1) {
        // uart_put_buffer("Hello, world!\r\n", 16);
        // uart_put_char('c');
        //if (uart_get_char(&test_byte)) {
        //    uart_put_char(test_byte);
        // }
        i2c_set_receive_address(SLAVE_ADDRESS, 0xFF);

        i2c_receive_string(SLAVE_ADDRESS);
        delimit_string((char *)i2c_receive_buffer);
        parse_data(output);



    }
}

void
set_rate(char *ubx_message, char *payload)
{
    fill_ubx_message(ubx_message,0x06,0x01,8,payload);
    // Ublox likes to send 255 + ACK from time to time so we set transmit to 17
    i2c_start_write(SLAVE_ADDRESS,17);

    i2c_write(SLAVE_ADDRESS,ubx_message,16);
    // Ask for a new piece of data
    LL_I2C_ClearFlag_STOP(I2C2);
    // Request write to i2c device
    LL_I2C_HandleTransfer(I2C2,
                          SLAVE_ADDRESS,
                          LL_I2C_ADDRSLAVE_7BIT,
                          1,
                          LL_I2C_MODE_AUTOEND,
                          LL_I2C_GENERATE_START_WRITE);
    // Transmit address to i2c device
    LL_I2C_TransmitData8(I2C2, 0xFF);
    while(LL_I2C_IsActiveFlag_BUSY(I2C2)) {}
    LL_I2C_HandleTransfer(I2C2,SLAVE_ADDRESS,
                          LL_I2C_ADDRSLAVE_7BIT,
                          1,
                          LL_I2C_MODE_AUTOEND,
                          LL_I2C_GENERATE_START_READ);
    int i = 0;
    char c;
    char flag = 0;
    while(!LL_I2C_IsActiveFlag_RXNE(I2C2)) {}
    while(i<10) {
        if(LL_I2C_IsActiveFlag_RXNE(I2C2)) {
            c = LL_I2C_ReceiveData8(I2C2);
            if(c == 0xb5) {
                flag = 1;
            }
            if(c != 255 && flag == 1) {
                i++;
            }
        }
        if(LL_I2C_IsActiveFlag_STOP(I2C2)) {
            i2c_continue_receiving(SLAVE_ADDRESS);
        }
    }

    LL_I2C_ClearFlag_STOP(I2C2);
    while(LL_I2C_IsActiveFlag_BUSY(I2C2)) {}
}
void
parse_data(uint8_t *output)
{
    //sometimes it does the "$" double while reading i2c
    if(!strcmp(strdelim[0], "$GPGSA") || !strcmp(strdelim[0], "$$GPGSA")) {
        //third variable in $GPGSA string is fix data, '3' is 3D-fix
        if(!strcmp(strdelim[2], "3"))
            readytosend = 1;

    } else if(!strcmp(strdelim[0],
                      "$GPGNS") || !strcmp(strdelim[0], "$$GPGNS")) {
        //markerid, utctime, latitude, NZ, longtitude, EW, altitude
        sprintf((char *)output,
                "%u,%s,%s,%s,%s,%s,%s",
                MARKER_ID,
                strdelim[1],
                strdelim[2],
                strdelim[3],
                strdelim[4],
                strdelim[5],
                strdelim[9]);
        readytosend++;
    }
    #ifdef REFERENCE
    else if(!strcmp(strdelim[0],
                    "$GPGBS") || !strcmp(strdelim[0], "$$GPGBS")) {
        sprintf((char *)output,
                "%s,%s,%s,%s",
                output,
                strdelim[2],
                strdelim[3],
                strdelim[4])
        readytosend++;
    }
    #else
    readytosend++;
    #endif
    if(readytosend >= 3) {}
}

void
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

//splits a delimited string into tokens
char *
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

void
fill_ubx_message(char    *ubx_buffer,
                 char    class_id,
                 char    message_id,
                 uint8_t length_of_payload,
                 char    *payload)
{
    uint8_t ck_a, ck_b, n;
    n = 6 + length_of_payload;
    ck_a = 0;
    ck_b = 0;
    ubx_buffer[0] = 0xB5;
    ubx_buffer[1] = 0x62;
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