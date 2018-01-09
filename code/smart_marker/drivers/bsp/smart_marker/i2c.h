#ifndef I2C_H
#define I2C_H

#include <board.h>
extern __IO uint8_t receiveIndex;
void i2c_init(uint8_t *buf, uint8_t buffersize);
void i2c_set_receive_address(uint32_t slave_address, uint8_t address);
void i2c_continue_receiving(uint32_t slave_address);
void i2c_error_callback(void);
void i2c_start_write(uint32_t slave_adress,uint8_t numbytes);
void i2c_write(uint32_t slave_address, uint8_t *message, uint8_t size);
#endif /* __I2C_H */