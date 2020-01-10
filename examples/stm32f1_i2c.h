#ifndef STM32F1_I2C_H
#define STM32F1_I2C_H

#include <libopencm3/stm32/i2c.h>

typedef struct i2c_info {
	uint32_t dev;
	uint8_t addr;
} i2c_info;

void write(void *conn_info, uint8_t *buffer, size_t size, bool is_cmd);

#endif
