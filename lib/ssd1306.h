#ifndef SSD1306_H
#define SSD1306_H
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

//TODO: read

#define SSD1306_I2C_ADDR1 0x3C
#define SSD1306_I2C_ADDR2 0x3D

#define SSD1306_MAX_GDDRAM 1024

/* Defines for fade command */
#define SSD1306_DISABLE_FADE		0x00 //1st argument | with FADE_FRAMES()
#define SSD1306_ENABLE_FADE		0x20 //1st argument | with FADE_FRAMES()
#define	SSD1306_ENABLE_BLINK		0x30 //1st argument | with FADE_FRAMES()
#define SSD1306_FADE_FRAMES(frames)	(((frames - 1)/8) & 0x0F) //1st argument

enum ssd1306_scroll_mode {
	ssd1306_right_horiz = 0x26,
	ssd1306_left_horiz = 0x27,
	ssd1306_vert_right_horiz = 0x29,
	ssd1306_vert_left_horiz = 0x2A
};

enum ssd1306_time_interval {
	ssd1306_5_frames,
	ssd1306_64_frames,
	ssd1306_128_frames,
	ssd1306_256_frames,
	ssd1306_3_frames,
	ssd1306_4_frames,
	ssd1306_25_frames,
	ssd1306_2_frames
};

enum ssd1306_addr_mode {
	ssd1306_horiz_a,
	ssd1306_vert_a,
	ssd1306_page_a
};

enum ssd1306_vccstate {
	ssd1306_external,
	ssd1306_switchcap
};

enum ssd1306_screen_type {
	ssd1306_128_32,
	ssd1306_128_64,
	ssd1306_96_16
};

#ifdef  __cplusplus

class SSD1306
{

public:
	SSD1306(void *conn_info,
		void (*write_ptr)(void *, uint8_t *, size_t, bool)) : 
		connection_info(conn_info), write_p(write_ptr) {};
		
	void default_init(enum ssd1306_screen_type type,
			  enum ssd1306_vccstate vs,
			  enum ssd1306_addr_mode mode);
			  
	void draw(uint8_t *buffer, size_t buffer_size);
	void display_power(bool power);
	void display_all_on(bool resume_from_ram);
	void display_invert(bool inverted);
	void display_clock_div(uint8_t div, uint8_t freq); //TODO
	void display_offset(uint8_t offset);
	void multiplex(uint8_t mux);
	void charge_pump(enum ssd1306_vccstate en);
	void precharge(uint8_t arg1); //TODO
	void pins(uint8_t arg1); //TODO
	void contrast(uint8_t contr);
	void memory_mode(enum ssd1306_addr_mode mode);
	void segment_remap(bool remap);
	void low_column(uint8_t column);
	void high_column(uint8_t column);
	void column_addr(uint8_t start_addr, uint8_t end_addr);
	void page_addr(uint8_t start_addr, uint8_t end_addr);
	void page_start_addr(uint8_t start_addr);
	void vcom_deselect(uint8_t arg1); //TODO
	void com_scan_dir(uint8_t arg1); //TODO
	void start_line(uint8_t line);
	void fade(uint8_t mode_and_rate); //See datasheet v1.5
	void zoom(bool en); //See datasheet v1.5
	void nop(void);
	void activate_scroll(void);
	void vertical_scroll_area(uint8_t arg1, uint8_t arg2); //TODO
	void horizontal_scroll(enum ssd1306_scroll_mode mode,
				uint8_t start_page,
				uint8_t stop_page,
				enum ssd1306_time_interval interval);

	void vertical_horizontal_scroll(enum ssd1306_scroll_mode mode,
					uint8_t start_page,
					uint8_t stop_page,
					enum ssd1306_time_interval interval);
	void stop_scroll(void);
	/*void start_scroll(enum ssd1306_scroll_mode mode,
				uint8_t start_page,
				uint8_t stop_page,
				enum ssd1306_time_interval interval);
	*/
	
private:
	void command(uint8_t *cmd, size_t cmd_size);
	void write(uint8_t *buffer, size_t size, bool is_cmd);
	
	//TODO: Make const?
	void *connection_info;
	void (*write_p)(void *, uint8_t *, size_t, bool);
};

#endif /* __cplusplus */

#ifdef __cplusplus
extern "C" {
#endif

	size_t sizeof_ssd1306(void);
	void new_ssd1306(void *ssd1306_obj,
			 void *conn_info,
			 void (*write_ptr)(void *, uint8_t *, size_t, bool));
			
	void ssd1306_default_init(void *ssd1306,
				  enum ssd1306_screen_type type,
				  enum ssd1306_vccstate vs,
				  enum ssd1306_addr_mode mode);
				  
	void ssd1306_draw(void *ssd1306, uint8_t *buffer, size_t buffer_size);
	void ssd1306_display_power(void *ssd1306, bool power);
	void ssd1306_display_all_on(void *ssd1306, bool resume_from_ram);
	void ssd1306_display_invert(void *ssd1306, bool inverted);
	
	void ssd1306_display_clock_div(void *ssd1306,
				       uint8_t div,
				       uint8_t freq);
				       
	void ssd1306_display_offset(void *ssd1306, uint8_t offset);
	void ssd1306_multiplex(void *ssd1306, uint8_t mux);
	void ssd1306_charge_pump(void *ssd1306, enum ssd1306_vccstate en);
	void ssd1306_precharge(void *ssd1306, uint8_t arg1); //TODO
	void ssd1306_pins(void *ssd1306, uint8_t arg1); //TODO
	void ssd1306_contrast(void *ssd1306, uint8_t contr);
	void ssd1306_memory_mode(void *ssd1306, enum ssd1306_addr_mode mode);
	void ssd1306_segment_remap(void *ssd1306, bool remap);
	void ssd1306_low_column(void *ssd1306, uint8_t column);
	void ssd1306_high_column(void *ssd1306, uint8_t column);
	
	void ssd1306_column_addr(void *ssd1306,
				 uint8_t start_addr,
				 uint8_t end_addr);
	
	void ssd1306_page_addr(void *ssd1306,
			       uint8_t start_addr,
			       uint8_t end_addr);
			       
	void ssd1306_page_start_addr(void *ssd1306, uint8_t start_addr);
	void ssd1306_vcom_deselect(void *ssd1306, uint8_t arg1); //TODO
	void ssd1306_com_scan_dir(void *ssd1306, uint8_t arg1); //TODO
	void ssd1306_start_line(void *ssd1306, uint8_t line);
	void ssd1306_fade(void *ssd1306, uint8_t mode_and_rate);
	void ssd1306_zoom(void *ssd1306, bool en);
	void ssd1306_nop(void *ssd1306);
	void ssd1306_activate_scroll(void *ssd1306);
	
	void ssd1306_vertical_scroll_area(void *ssd1306,
					  uint8_t arg1,
					  uint8_t arg2); //TODO
	
	void ssd1306_stop_scroll(void *ssd1306);
/*
	void ssd306_start_scroll(void *ssd1306,
				enum ssd1306_scroll_mode mode,
				uint8_t start_page,
				uint8_t stop_page,
				enum ssd1306_time_interval interval); */
#ifdef __cplusplus
}
#endif
#endif /* SSD1306_H */
