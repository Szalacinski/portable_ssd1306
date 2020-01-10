#ifndef SSD1306_STM32F1_I2C
#define SSD1306_STM32F1_I2C
#include "stm32f1_i2c.h"

void write(void *conn_info, uint8_t *buffer, size_t size, bool is_cmd)
{
	uint32_t dev = ((const i2c_info *)conn_info)->dev;
	uint8_t addr = ((const i2c_info *)conn_info)->addr;

	while ((I2C_SR2(dev) & I2C_SR2_BUSY)) {
	}
		
	i2c_send_start(dev);

	/* Wait for master mode selected */
	while (!((I2C_SR1(dev) & I2C_SR1_SB)
		& (I2C_SR2(dev) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

	i2c_send_7bit_address(dev, addr, I2C_WRITE);

	/* Waiting for address is transferred. */
	while (!(I2C_SR1(dev) & I2C_SR1_ADDR));

	/* Clearing ADDR condition sequence. */
	(void)I2C_SR2(dev);
		/* Send control byte */
		i2c_send_data(dev, is_cmd ? 0x00 : 0x40);
		while (!(I2C_SR1(dev) & (I2C_SR1_BTF)));
	for (size_t i = 0; i < size; i++) {
		i2c_send_data(dev, buffer[i]);
		while (!(I2C_SR1(dev) & (I2C_SR1_BTF)));
	}
	i2c_send_stop(dev);
}

#endif
