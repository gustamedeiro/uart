#ifndef SSD1306_H
#define SSD1306_H

#include "hardware/i2c.h"

typedef struct {
    i2c_inst_t *i2c;
    uint8_t address;
    int width, height;
} ssd1306_t;

void ssd1306_init(ssd1306_t* ssd, int width, int height, bool vcc, uint8_t address, i2c_inst_t *i2c);
void ssd1306_clear(ssd1306_t* ssd);
void ssd1306_draw_string(ssd1306_t* ssd, const char* str, int x, int y);
void ssd1306_send_data(ssd1306_t* ssd);

#endif // SSD1306_H
