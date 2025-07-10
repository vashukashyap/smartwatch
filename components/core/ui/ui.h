#pragma once

#include <stdint.h>
#include <display.h>


typedef  struct
{
    uint8_t x;
    uint8_t y;
    uint8_t width;
    uint8_t height;
    uint8_t radius;
    uint8_t padding_x;
    uint8_t padding_y;
    uint16_t color;
    uint16_t text_color;
    const GFXfont *font;
    char *text;
} rectangle_widget_t;

typedef  struct
{
    uint8_t x;
    uint8_t y;
    uint8_t thickness;
    uint8_t start_angle;
    uint8_t end_angle;
    uint8_t radius;
    uint8_t padding_x;
    uint8_t padding_y;
    uint16_t color;
    uint16_t text_color;
    uint16_t text_angle;
    const GFXfont *font;
    char *text;
} arc_widget_t;



void rectangle_widget(rectangle_widget_t *rectangle_widget_config);

void arc_widget(arc_widget_t *arc_widget_config);




