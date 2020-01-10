#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/gpio.h>
#include <string.h>
#include "stm32f1_i2c.h"
#include "ssd1306_lib/ssd1306.h"

/* Setup the connection info struct */
static const i2c_info info = {I2C2, SSD1306_I2C_ADDR1};
static const enum ssd1306_screen_type type = ssd1306_128_32;
static const uint16_t height = 32;
static const uint16_t width = 128;

static void i2c_setup(void)
{
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_I2C2);
	rcc_periph_clock_enable(RCC_AFIO);

	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
			GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,
			GPIO_I2C2_SCL | GPIO_I2C2_SDA);

	i2c_peripheral_disable(I2C2);
	i2c_set_speed(I2C2, i2c_speed_fm_400k, I2C_CR2_FREQ_32MHZ);
	i2c_peripheral_enable(I2C2);
}

static void delay(void *ssd1306)
{
	for (uint16_t i = 0; i < 128*64; ++i)
		ssd1306_nop(ssd1306);
}

int main(void)
{
	/* Reserve space for the wrapped object */
	uint8_t ssd1306_obj[sizeof_ssd1306()];
	void *ssd1306_ptr = (void *)ssd1306_obj;
	/* Display buffer */
	uint8_t buf[(width * height) / 8];
	bool tmp = 0;

	rcc_clock_setup_in_hse_8mhz_out_72mhz();
	i2c_setup();

	/* Fill the reserved space with the object
	 * pass the connection info, and pass the
	 * "write" function as a function pointer.
	 * To port this library to a new platform
	 * or protocol, a write function must be
	 * written that takes a pointer to a
	 * user-defined struct filled with connection
	 * info. In this demo, we are using I2C with an
	 * STM32, and our defined info structure.
	 */
	new_ssd1306(ssd1306_ptr, (void *)&info, write);

	/* fill buffer with lines */
	memset(buf, 0xAA, sizeof(buf));

	ssd1306_default_init(ssd1306_ptr, type, ssd1306_switchcap);

	ssd1306_draw(ssd1306_ptr, buf, sizeof(buf));

	/* Make the screen appear as if it is slightly wiggling */
	while (1) {
		ssd1306_display_invert(ssd1306_ptr, tmp);
		tmp ^= 1;
		delay(ssd1306_ptr);
	}
	return 0;
}
