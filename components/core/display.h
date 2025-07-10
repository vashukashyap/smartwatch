#pragma once

#include <stdint.h>
#include <stdbool.h>


typedef struct {
  uint16_t bitmapOffset;
  uint8_t  width;
  uint8_t  height;
  uint8_t  xAdvance;
  int8_t   xOffset;
  int8_t   yOffset;
} GFXglyph;

typedef struct {
  const uint8_t  *bitmap;
  const GFXglyph *glyph;
  uint8_t   first;
  uint8_t   last;
  uint8_t   yAdvance;
} GFXfont;


void display_draw_hline(uint16_t x, uint16_t y, uint16_t length, uint16_t color);

void display_draw_rectangle(uint16_t pos_x, uint16_t pos_y, uint16_t width, uint16_t height, uint16_t color);

void display_draw_circle(uint16_t pos_x,uint16_t pos_y, uint16_t radius, uint16_t color);

void display_draw_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color);

// void display_draw_circle_quadrant_filled(uint16_t x0, uint16_t y0, uint16_t r, uint8_t quadrant, uint16_t color);

void display_draw_rounded_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t radius, uint16_t color);

void display_draw_line(int x0, int y0, int x1, int y1, uint16_t color, uint8_t thickness);

void v_set_pixel(int16_t x, int16_t y, uint16_t color, uint8_t thickness);

void v_display_draw_rectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t color);

void v_display_draw_circle(uint8_t cx, uint8_t cy, uint8_t radius, uint16_t color, bool filled);

void v_display_draw_round_rectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t r, uint16_t color);

void v_display_draw_line(int x0, int y0, int x1, int y1, uint16_t color, uint8_t thickness);

void v_draw_char_gfx(int16_t x, int16_t y, char c, const GFXfont *font, uint16_t color);

void v_draw_text_gfx(int16_t x, int16_t y, const char *str, const GFXfont *font, uint16_t color);


// void v_draw_arc_hline(uint16_t cx, uint16_t cy,
//                       uint16_t inner_r, uint16_t outer_r,
//                       float start_angle_deg, float end_angle_deg,
//                       uint16_t color);

// void v_display_draw_arc_m(uint8_t cx, uint8_t cy, uint8_t radius_i, uint8_t radius_o, uint8_t start_a, uint8_t end_a, uint16_t color, bool filled);

void v_display_draw_arc(int16_t cx, int16_t cy, int16_t r, int16_t thickness, float start_angle, float end_angle, uint16_t color);

void v_draw_char_gfx_rotated(int16_t x, int16_t y, char c, const GFXfont *font, uint16_t color, float angle_deg);

void v_draw_text_gfx_rotated(int16_t x, int16_t y, const char *str, const GFXfont *font, uint16_t color, float angle_deg);


void v_draw_bitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint16_t w, uint16_t h, uint16_t color);

void v_draw_rgb565_image(int x, int y, int w, int h, const uint8_t *img_data);