#include <stdarg.h>
#include <stdbool.h>
#include <new>
#include "ssd1306.h"

/*
 * A common (but still ugly) hack to count the number of arguments,
 * because for some reason this is still not a language feature.
 */
#define PP_NARG(...) PP_NARG_(__VA_ARGS__,PP_RSEQ_N())
#define PP_NARG_(...) PP_ARG_N(__VA_ARGS__)
#define PP_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define PP_RSEQ_N() 10,9,8,7,6,5,4,3,2,1,0

/* 
 * Sets up a command with the arguments passed.
 * The fact that it counts the number of arguments 
 * helps prevent programmer errors.
 */
#define INIT_COMMAND(...) uint8_t arr[PP_NARG(__VA_ARGS__)] = {__VA_ARGS__}

/* Sends the command set up in INIT_COMMAND */
#define SEND_COMMAND() command(arr, sizeof(arr))

/* A one-shot macro to set up and send the command */
#define COMMAND(...) INIT_COMMAND(__VA_ARGS__); SEND_COMMAND();

/* Using the COMMAND macro with type conversions results in a warning.
 * Use this for single-byte commands to explicitly save stack space.
 */
#define BYTE_COMMAND(cmd) uint8_t tmp = (cmd); command(&tmp, 1);

/*
 * A simple macro to trim down long lines
 * when casting pointers and calling methods.
 */
#define SSD1306_CALL_CPP(ssd1306, x) reinterpret_cast<SSD1306*>(ssd1306)->x

/* Fundamental Commands */
constexpr uint8_t SSD1306_SETCONTRAST = 0x81;
constexpr uint8_t SSD1306_DISPLAYALLONRESUME = 0xA4;
constexpr uint8_t SSD1306_DISPLAYALLON = 0xA5;
constexpr uint8_t SSD1306_NORMALDISPLAY = 0xA6;
constexpr uint8_t SSD1306_INVERTDISPLAY = 0xA7;
constexpr uint8_t SSD1306_DISPLAYOFF = 0xAE;
constexpr uint8_t SSD1306_DISPLAYON = 0xAF;
/* Scrolling Commands */
constexpr uint8_t SSD1306_RIGHT_HORIZ_SCROLL = 0x26;
constexpr uint8_t SSD1306_LEFT_HORIZ_SCROLL = 0x27;
constexpr uint8_t SSD1306_VERT_RIGHT_HORIZ_SCROLL = 0x29;
constexpr uint8_t SSD1306_VERT_LEFT_HORIZ_SCROLL = 0x2A;
constexpr uint8_t SSD1306_DEACTIVATE_SCROLL = 0x2E;
constexpr uint8_t SSD1306_ACTIVATE_SCROLL = 0x2F;
constexpr uint8_t SSD1306_SET_VERTICAL_SCROLL_AREA = 0xA3;
/* Addressing Commands */
constexpr uint8_t SSD1306_SETLOWCOLUMN = 0x00;
constexpr uint8_t SSD1306_SETHIGHCOLUMN = 0x10;
constexpr uint8_t SSD1306_MEMORYMODE = 0x20;
constexpr uint8_t SSD1306_SETCOLUMNADDR = 0x21;
constexpr uint8_t SSD1306_SETPAGEADDR = 0x22;
constexpr uint8_t SSD1306_SETPAGESTARTADDR = 0xB0;
/* Hardware Configuration Commands */
constexpr uint8_t SSD1306_SETSTARTLINE = 0x40;
constexpr uint8_t SSD1306_SEGREMAP = 0xA0;
constexpr uint8_t SSD1306_SETMULTIPLEX = 0xA8;
constexpr uint8_t SSD1306_COMSCANINC = 0xC0;
constexpr uint8_t SSD1306_COMSCANDEC = 0xC8;
constexpr uint8_t SSD1306_SETDISPLAYOFFSET = 0xD3;
constexpr uint8_t SSD1306_SETCOMPINS = 0xDA;
/* Timing Commands */
constexpr uint8_t SSD1306_SETDISPLAYCLOCKDIV = 0xD5;
constexpr uint8_t SSD1306_SETPRECHARGE = 0xD9;
constexpr uint8_t SSD1306_SETVCOMDESELECT = 0xDB;
constexpr uint8_t SSD1306_NOP = 0xE3;
/* Advanced Graphics Commands */
constexpr uint8_t SSD1306_FADE = 0x23;
constexpr uint8_t SSD1306_ZOOM = 0xD6;
/* Charge Pump Command */
constexpr uint8_t SSD1306_CHARGEPUMP = 0x8D;



size_t sizeof_ssd1306(void)
{
	return sizeof(SSD1306);
}



void new_ssd1306(void *ssd1306_obj, void *conn_info,
			void (*write_ptr)(void *, uint8_t *, size_t, bool))
{
	new(ssd1306_obj) SSD1306(conn_info, write_ptr);
}


/* The default init code.  99% of the time you will use
 * this if you aren't doing anything particuarly crazy.
 */
void SSD1306::default_init(enum ssd1306_screen_type type,
			   enum ssd1306_vccstate vs,
			   enum ssd1306_addr_mode mode)
{
	uint8_t compins, cont, height, width;
	switch (type) {
		case ssd1306_128_32:
			compins = 0x02;
			cont = 0x8F;
			height = 32;
			width = 128;
			break;
		case ssd1306_128_64:
			compins = 0x12;
			cont = vs ?  0xCF : 0x9F;
			height = 64;
			width = 128;
			break;
		case ssd1306_96_16:
			compins = 0x02;
			cont = vs ? 0xAF : 0x10;
			height = 16;
			width = 96;
			break;
	}
	display_power(0);
	multiplex(height - 1);
	display_offset(0);
	start_line(0);
	segment_remap(1);
	com_scan_dir(0);
	pins(compins);
	contrast(cont);
	display_clock_div(0x80);
	charge_pump(vs);
	
	memory_mode(mode);
	
	if (mode == ssd1306_page_a) {
		//TODO: set higher column start addr
		//TODO: set page start addr
	} else {
		//TODO: set column addr
		//TODO: set page addr
	}
	
	precharge(vs ? 0xF1 : 0x22);
	vcom_deselect(0x40);
	display_all_on(1);
	display_invert(0);
	stop_scroll();
	fade(SSD1306_DISABLE_FADE);
	zoom(0);
	display_power(1);
}

void ssd1306_default_init(void *ssd1306,
			  enum ssd1306_screen_type type,
			  enum ssd1306_vccstate vs,
			  enum ssd1306_addr_mode mode)
{
	SSD1306_CALL_CPP(ssd1306, default_init(type, vs, mode));
}



void SSD1306::write(uint8_t *buffer, size_t size, bool is_cmd)
{
	write_p(connection_info, buffer, size, is_cmd);
}



void SSD1306::command(uint8_t *cmd, size_t cmd_size)
{
	write(cmd, cmd_size, 1);
}



void SSD1306::draw(uint8_t *buffer, size_t buffer_size)
{
	write(buffer, buffer_size, 0);
}

void ssd1306_draw(void *ssd1306, uint8_t *buffer, size_t buffer_size)
{
	SSD1306_CALL_CPP(ssd1306, draw(buffer, buffer_size));
}



void SSD1306::display_power(bool power)
{
	COMMAND(power ? SSD1306_DISPLAYON : SSD1306_DISPLAYOFF);
}

void ssd1306_display_power(void *ssd1306, bool power)
{
	SSD1306_CALL_CPP(ssd1306, display_power(power));
}



void SSD1306::display_all_on(bool resume_from_ram)
{
	
	COMMAND(resume_from_ram ? SSD1306_DISPLAYALLONRESUME : SSD1306_DISPLAYALLON);
}

void ssd1306_display_all_on(void *ssd1306, bool resume_from_ram)
{
	SSD1306_CALL_CPP(ssd1306, display_all_on(resume_from_ram));
}



void SSD1306::display_invert(bool inverted)
{
	COMMAND(inverted ? SSD1306_INVERTDISPLAY : SSD1306_NORMALDISPLAY);
}

void ssd1306_display_invert(void *ssd1306, bool inverted)
{
	SSD1306_CALL_CPP(ssd1306, display_invert(inverted));
}



void SSD1306::display_clock_div(uint8_t div, uint8_t freq)
{
	COMMAND(SSD1306_SETDISPLAYCLOCKDIV, (freq << 4) | (0x0F & div));
}

void ssd1306_display_clock_div(void *ssd1306, uint8_t div, uint8_t freq)
{
	SSD1306_CALL_CPP(ssd1306, display_clock_div(div, freq));
}



void SSD1306::display_offset(uint8_t offset)
{
	COMMAND(SSD1306_SETDISPLAYOFFSET, offset);
}

void ssd1306_display_offset(void *ssd1306, uint8_t offset)
{
	SSD1306_CALL_CPP(ssd1306, display_offset(offset));
}



void SSD1306::multiplex(uint8_t mux)
{
	COMMAND(SSD1306_SETMULTIPLEX, mux);
}

void ssd1306_multiplex(void *ssd1306, uint8_t mux)
{
	SSD1306_CALL_CPP(ssd1306, multiplex(mux));
}



void SSD1306::charge_pump(enum ssd1306_vccstate en)
{
	uint8_t tmp = en ? 0x14 : 0x10;
	COMMAND(SSD1306_CHARGEPUMP, tmp);
}

void ssd1306_charge_pump(void *ssd1306, enum ssd1306_vccstate en)
{
	SSD1306_CALL_CPP(ssd1306, charge_pump(en));
}



void SSD1306::precharge(uint8_t arg1)
{
	COMMAND(SSD1306_SETPRECHARGE, arg1);
}

void ssd1306_precharge(void *ssd1306, uint8_t arg1)
{
	SSD1306_CALL_CPP(ssd1306, precharge(arg1));
}



void SSD1306::pins(uint8_t arg1)
{
	COMMAND(SSD1306_SETCOMPINS, arg1);
}

void ssd1306_pins(void *ssd1306, uint8_t arg1)
{
	SSD1306_CALL_CPP(ssd1306, pins(arg1));
}



void SSD1306::contrast(uint8_t contr)
{
	COMMAND(SSD1306_SETCONTRAST, contr);
}

void ssd1306_contrast(void *ssd1306, uint8_t contr)
{
	SSD1306_CALL_CPP(ssd1306, contrast(contr));
}



void SSD1306::memory_mode(enum ssd1306_addr_mode mode)
{
	uint8_t tmp = (uint8_t)mode;
	COMMAND(SSD1306_MEMORYMODE, tmp);
}

void ssd1306_memory_mode(void *ssd1306, enum ssd1306_addr_mode mode)
{
	SSD1306_CALL_CPP(ssd1306, memory_mode(mode));
}



void SSD1306::segment_remap(bool remap)
{
	BYTE_COMMAND(SSD1306_SEGREMAP | (0x01 & arg1));
}

void ssd1306_segment_remap(void *ssd1306, bool remap)
{
	SSD1306_CALL_CPP(ssd1306, segment_remap(arg1));
}

void SSD1306::low_column(uint8_t column)
{
	BYTE_COMMAND(0x0F & column);
}

void ssd1306_low_column(void *ssd1306, uint8_t column)
{
	SSD1306_CALL_CPP(ssd1306, low_column(column));
}



void SSD1306::high_column(uint8_t column)
{
	BYTE_COMMAND(SSD1306_SETHIGHCOLUMN | (0x0F & column));
}

void ssd1306_high_column(void *ssd1306, uint8_t column)
{
	SSD1306_CALL_CPP(ssd1306, high_column(column));
}



void SSD1306::column_addr(uint8_t start_addr, uint8_t end_addr)
{
	COMMAND(SSD1306_SETCOLUMNADDR, start_addr, end_addr);
}

void ssd1306_column_addr(void *ssd1306, uint8_t start_addr, uint8_t end_addr)
{
	SSD1306_CALL_CPP(ssd1306, column_addr(start_addr, end_addr));
}



void SSD1306::page_addr(uint8_t start_addr, uint8_t end_addr)
{
	COMMAND(SSD1306_SETCOLUMNADDR, start_addr, end_addr);
}

void ssd1306_page_addr(void *ssd1306, uint8_t start_addr, uint8_t end_addr)
{
	SSD1306_CALL_CPP(ssd1306, page_addr(start_addr, end_addr));
}



void SSD1306::page_start_addr(uint8_t start_addr)
{
	BYTE_COMMAND(SSD1306_SETPAGESTARTADDR | (0xF8 & start_addr));
}

void ssd1306_page_start_addr(void *ssd1306, uint8_t start_addr)
{
	SSD1306_CALL_CPP(ssd1306, page_start_addr(start_addr));
}



void SSD1306::vcom_deselect(uint8_t arg1)
{
	COMMAND(SSD1306_SETVCOMDESELECT, arg1);
}

void ssd1306_vcom_deselect(void *ssd1306, uint8_t arg1)
{
	SSD1306_CALL_CPP(ssd1306, vcom_deselect(arg1));
}



void SSD1306::com_scan_dir(uint8_t arg1)
{
	COMMAND(arg1 ? SSD1306_COMSCANINC : SSD1306_COMSCANDEC);
}

void ssd1306_com_scan_dir(void *ssd1306, uint8_t arg1)
{
	SSD1306_CALL_CPP(ssd1306, com_scan_dir(arg1));
}



void SSD1306::start_line(uint8_t line)
{
	BYTE_COMMAND(SSD1306_SETSTARTLINE | (0x3F & line));
}

void ssd1306_start_line(void *ssd1306, uint8_t line)
{
	SSD1306_CALL_CPP(ssd1306,  start_line(line));
}



void SSD1306::activate_scroll(void)
{
	COMMAND(SSD1306_ACTIVATE_SCROLL);
}

void ssd1306_activate_scroll(void *ssd1306)
{
	SSD1306_CALL_CPP(ssd1306, activate_scroll());
}


//TODO: private?
void SSD1306::vertical_scroll_area(uint8_t arg1, uint8_t arg2)
{
	COMMAND(SSD1306_SET_VERTICAL_SCROLL_AREA, arg1, arg2);
}

void ssd1306_vertical_scroll_area(void *ssd1306, uint8_t arg1, uint8_t arg2)
{
	SSD1306_CALL_CPP(ssd1306, vertical_scroll_area(arg1, arg2));
}



void SSD1306::horizontal_scroll(enum ssd1306_scroll_mode mode,
				uint8_t start_page,
				uint8_t stop_page,
				enum ssd1306_time_interval interval)
{
	COMMAND((uint8_t)mode, 0x00, start_page, (uint8_t)interval, stop_page, 0x00, 0xFF);
}

void SSD1306::vertical_horizontal_scroll(enum ssd1306_scroll_mode mode,
					uint8_t start_page,
					uint8_t stop_page,
					enum ssd1306_time_interval interval)
{
	//TODO: Vertical scrolling offset?
	COMMAND((uint8_t)mode, 0X00, start_page, (uint8_t)interval, stop_page, 0X01);

}
/*
void SSD1306::start_scroll(enum ssd1306_scroll_mode mode,
				uint8_t start_page,
				uint8_t stop_page,
				enum ssd1306_time_interval interval)
{
	switch(mode) {
		case (ssd1306_right_horiz || ssd1306_left_horiz):
			horizontal_scroll(mode, start_page, stop_page, interval);
			activate_scroll();
			break;
		case (ssd1306_vert_right_horiz || ssd1306_vert_left_horiz):
			vertical_scroll_area(0X00, height);
			vertical_horizontal_scroll(mode, start_page, stop_page, interval);
			activate_scroll();
			break;
	}
}

void ssd1306_start_scroll(void *ssd1306,
{
	SSD1306_CALL_CPP(ssd1306, 
}
*/


//GDDRAM needs to be rewritten after this command
void SSD1306::stop_scroll(void)
{
	COMMAND(SSD1306_DEACTIVATE_SCROLL);
}

void ssd1306_stop_scroll(void *ssd1306)
{
	SSD1306_CALL_CPP(ssd1306, stop_scroll());
}



void SSD1306::fade(uint8_t mode_and_rate)
{
	COMMAND(SSD1306_FADE, mode_and_rate);
}

void ssd1306_fade(void *ssd1306, uint8_t mode_and_rate)
{
	SSD1306_CALL_CPP(ssd1306, fade(mode_and_rate));
}



void SSD1306::zoom(bool en)
{
	COMMAND(SSD1306_ZOOM, en);
}

void ssd1306_zoom(void *ssd1306, bool en)
{
	SSD1306_CALL_CPP(ssd1306, zoom(en));
}



void SSD1306::nop(void)
{
	COMMAND(SSD1306_NOP);
}

void ssd1306_nop(void *ssd1306)
{
	SSD1306_CALL_CPP(ssd1306, nop());
}
