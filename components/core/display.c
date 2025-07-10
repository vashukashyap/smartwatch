#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <gc9a01a.h>
#include <esp_log.h>
#include <stdbool.h>
#include <display.h>



static const char *TAG = "Display";

extern uint8_t* v_display_buffer; 

void display_draw_hline(uint16_t x, uint16_t y, uint16_t length, uint16_t color)
{
    if (y >= 240 || x >= 240) return;

    if (x + length > 240)
        length = 240 - x;

    uint8_t line_buffer[length * 2];
    for (int i = 0; i < length; i++) {
        line_buffer[i * 2]     = (color >> 8) & 0xFF;
        line_buffer[i * 2 + 1] = color & 0xFF;
    }

    gc9a01a_draw_cursor_set(x, y, x + length - 1, y);
    gc9a01a_send_cmd(0x2C);
    gc9a01a_send_data(line_buffer, length * 2);
}


void display_draw_rectangle(uint16_t pos_x, uint16_t pos_y, uint16_t width, uint16_t height, uint16_t color)
{
  if(pos_x+ width>240){
    ESP_LOGW(TAG, "width is more then 240. (width should be less then 240)");
    return;
  }

  if(pos_y + height>240){
    ESP_LOGW(TAG, "height is more then 240. (height should be less then 240)");
    return;
  }

  uint8_t line_buffer[width*2];

  for(int i=0; i<width; i++)
  {
    line_buffer[i*2] = (color >> 8) & 0xFF;
    line_buffer[i*2+1] = (color) & 0xFF;
  }

  gc9a01a_draw_cursor_set(pos_x,pos_y,pos_x+width-1,pos_y+height-1);

  gc9a01a_send_cmd(0x2C);
  for(int i=0; i< height; i++)
  {
      gc9a01a_send_data(line_buffer, width*2);
  }
  
//   gc9a01a_send_v_display_buffer(v_display_buffer);
}

void display_draw_circle(uint16_t pos_x,uint16_t pos_y , uint16_t radius, uint16_t color)
{
   if (pos_x >= 240 || pos_y >= 240 || radius == 0) return;

    int x = 0;
    int y = radius;
    int d = 1 - radius;

    while (y >= x)
    {
        display_draw_hline(pos_x - x, pos_y - y, 2 * x + 1, color);
        display_draw_hline(pos_x - y, pos_y - x, 2 * y + 1, color);
        display_draw_hline(pos_x - x, pos_y + y, 2 * x + 1, color);
        display_draw_hline(pos_x - y, pos_y + x, 2 * y + 1, color);

        x++;
        if (d < 0)
            d += 2 * x + 1;
        else {
            y--;
            d += 2 * (x - y) + 1;
        }
    }
}


void display_draw_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color)
{
    float invslope1 = (float)(x1 - x0) / (y1 - y0);
    float invslope2 = (float)(x2 - x0) / (y2 - y0);

    float curx1 = x0;
    float curx2 = x0;

    for (int y = y0; y <= y1; y++) {
        int x_start = (int)curx1;
        int x_end   = (int)curx2;
        if (x_start > x_end) {
            int tmp = x_start; x_start = x_end; x_end = tmp;
        }

        display_draw_hline(x_start, y, x_end - x_start + 1, color);
        curx1 += invslope1;
        curx2 += invslope2;
    }
}


void display_draw_rounded_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t radius, uint16_t color)
{
    if (w < 2 * radius || h < 2 * radius) {
        ESP_LOGW(TAG, "Rounded corners too big for rectangle!");
        return;
    }

    // Center rectangle (fill)
    display_draw_rectangle(x + radius, y, w - 2 * radius, h, color);

    // Left and right vertical bars
    display_draw_rectangle(x, y + radius, radius, h - 2 * radius, color);
    display_draw_rectangle(x + w - radius, y + radius, radius, h - 2 * radius, color);

    // // Rounded corners (filled quarter circles)
    display_draw_circle(x + radius, y + radius, radius, color);  // top-left
    display_draw_circle(x + w - radius - 1, y + radius, radius, color);  // top-right
    display_draw_circle(x + radius, y + h - radius - 1, radius, color);  // bottom-left
    display_draw_circle(x + w - radius - 1, y + h - radius - 1, radius, color);  // bottom-right
}

void display_draw_line(int x0, int y0, int x1, int y1, uint16_t color, uint8_t thickness)
{
    int dx = abs(x1 - x0);
    int dy = -abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;

    // Set address window for the whole display once (worst case)
    gc9a01a_draw_cursor_set(0, 0, GC9A01A_TFTWIDTH - 1, GC9A01A_TFTHEIGHT - 1);
    gc9a01a_send_cmd(0x2C);  // Memory write

    // Precompute color bytes
    uint8_t high = color >> 8;
    uint8_t low  = color & 0xFF;
    uint8_t pixel[2] = { high, low };

    while (1) {
        for (int i = -thickness / 2; i <= thickness / 2; i++) {
            for (int j = -thickness / 2; j <= thickness / 2; j++) {
                int px = x0 + i;
                int py = y0 + j;

                if (px >= 0 && px < GC9A01A_TFTWIDTH && py >= 0 && py < GC9A01A_TFTHEIGHT) {
                    // Set cursor for this pixel
                    gc9a01a_draw_cursor_set(px, py, px, py);
                    gc9a01a_send_cmd(0x2C);
                    gc9a01a_send_data(pixel, 2);
                }
            }
        }

        if (x0 == x1 && y0 == y1) break;

        int e2 = 2 * err;

        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }

        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}


void v_set_pixel(int16_t x, int16_t y, uint16_t color, uint8_t thickness) {
    if (!v_display_buffer || thickness == 0) return;

    int half = thickness / 2;

    for (int dy = -half; dy <= half; dy++) {
        for (int dx = -half; dx <= half; dx++) {
            int16_t px = x + dx;
            int16_t py = y + dy;

            // ✅ Prevent writing out of bounds
            if (px >= 0 && px <= GC9A01A_TFTWIDTH -1 && py >= 0 && py <= GC9A01A_TFTHEIGHT-1) {
                uint32_t index = (py * GC9A01A_TFTWIDTH + px) * 2;
                v_display_buffer[index]     = color >> 8;
                v_display_buffer[index + 1] = color & 0xFF;
            }
        }
    }

}


void v_display_draw_rectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t color)
{
    if (!v_display_buffer)
    {
        ESP_LOGW(TAG, "v_display_buffer is NULL.");
        return;
    }

    if(x+width > GC9A01A_TFTWIDTH || y+height > GC9A01A_TFTHEIGHT)
    {
         ESP_LOGW(TAG, "out of bond values.");
        return;
    }

    for (uint8_t j = 0; j < height; j++)
    {
        for (uint8_t i = 0; i < width; i++)
        {

            uint8_t px = x + i;
            uint8_t py = y + j;

            uint32_t index = (py * GC9A01A_TFTWIDTH + px) * 2;
            v_display_buffer[index] = color >> 8 & 0xFF;
            v_display_buffer[index + 1] = color & 0xFF;
        }
    }

    // gc9a01a_send_v_display_buffer(v_display_buffer);
}


void v_display_draw_circle(uint8_t cx, uint8_t cy, uint8_t radius, uint16_t color, bool filled) {
    if (!v_display_buffer) {
        ESP_LOGW(TAG, "v_display_buffer is NULL.");
        return;
    }

    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (x <= y) {
        if (filled) {
            // Draw vertical lines between top and bottom
            for (int i = cx - x; i <= cx + x; i++) {
                
                v_set_pixel(i, cy - y, color, 1);
                v_set_pixel(i, cy + y, color, 1);
            }
            for (int i = cx - y; i <= cx + y; i++) {
                v_set_pixel(i, cy - x, color, 1);
                v_set_pixel(i, cy + x, color, 1);
            }
        } else {
            // Outline only
            v_set_pixel(cx + x, cy + y, color, 1);
            v_set_pixel(cx - x, cy + y, color, 1);
            v_set_pixel(cx + x, cy - y, color, 1);
            v_set_pixel(cx - x, cy - y, color, 1);
            v_set_pixel(cx + y, cy + x, color, 1);
            v_set_pixel(cx - y, cy + x, color, 1);
            v_set_pixel(cx + y, cy - x, color, 1);
            v_set_pixel(cx - y, cy - x, color, 1);
        }

        if (d < 0) {
            d += 4 * x + 6;
        } else {
            d += 4 * (x - y) + 10;
            y--;
        }
        x++;

    }

    // gc9a01a_send_v_display_buffer(v_display_buffer);  // update display
}

void v_display_draw_round_rectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height,
                                uint8_t r, uint16_t color) {
    if (!v_display_buffer) {
        ESP_LOGW(TAG, "v_display_buffer is NULL");
        return;
    }

    // Clamp radius
    if (r * 2 > width)  r = width / 2;
    if (r * 2 > height) r = height / 2;

    // Draw center rectangle
    for (uint8_t j = y + r; j < y + height - r; j++) {
        for (uint8_t i = x; i < x + width; i++) {
            v_set_pixel(i, j, color, 1);
        }
    }

    // Draw top and bottom horizontal bars
    for (uint8_t j = y; j < y + r; j++) {
        for (uint8_t i = x + r; i < x + width - r; i++) {
            v_set_pixel(i, j, color, 1);  // Top bar
        }
    }
    for (uint8_t j = y + height - r; j < y + height; j++) {
        for (uint8_t i = x + r; i < x + width - r; i++) {
            v_set_pixel(i, j, color, 1);  // Bottom bar
        }
    }

    // Draw corner circles
    v_display_draw_circle(x+r, y+r, r, color, 1);
    v_display_draw_circle(x+width-r-1, y+r, r, color, 1);
    v_display_draw_circle(x+r+1, y+height-r-1, r, color, 1);
    v_display_draw_circle(x+width-r-1, y+height-r-1, r, color, 1);

    // gc9a01a_send_v_display_buffer(v_display_buffer);
}

void v_display_draw_line(int x0, int y0, int x1, int y1, uint16_t color, uint8_t thickness)
{
    if (!v_display_buffer) {
        ESP_LOGW(TAG, "v_display_buffer is NULL");
        return;
    }

    int dx = abs(x1 - x0);
    int dy = -abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;

    while (1) {
        // Draw thickness using square around (x0, y0)
        for (int i = -thickness / 2; i <= thickness / 2; i++) {
            for (int j = -thickness / 2; j <= thickness / 2; j++) {
                v_set_pixel(x0 + i, y0 + j, color, 1);
            }
        }

        if (x0 == x1 && y0 == y1) break;

        int e2 = 2 * err;

        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }

        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }

    // gc9a01a_send_v_display_buffer(v_display_buffer);
}


void v_draw_char_gfx(int16_t x, int16_t y, char c, const GFXfont *font, uint16_t color) {
    if (c < font->first || c > font->last) return;

    const GFXglyph *glyph = &font->glyph[c - font->first];
    const uint8_t *bitmap = font->bitmap + glyph->bitmapOffset;

    uint8_t  w  = glyph->width;
    uint8_t  h  = glyph->height;
    int8_t   xo = glyph->xOffset;
    int8_t   yo = glyph->yOffset;

    int16_t x_start = x + xo;
    int16_t y_start = y + yo;

    uint8_t bit = 0, bits = 0;

    for (uint8_t yy = 0; yy < h; yy++) {
        for (uint8_t xx = 0; xx < w; xx++) {
            if (!(bit--)) {
                bits = *bitmap++;
                bit = 7;
            }
            if (bits & 0x80) {
                v_set_pixel(x_start + xx, y_start + yy, color, 1);
            }
            bits <<= 1;
        }
    }

    // Optional: buffer flush if needed per-char
    // gc9a01a_send_v_display_buffer(v_display_buffer);
}


void v_draw_text_gfx(int16_t x, int16_t y, const char *str, const GFXfont *font, uint16_t color)
{
    int16_t cursor_x = x;
    int16_t cursor_y = y;

    while (*str) {
        if (*str == '\n') {
            cursor_y += font->yAdvance;
            cursor_x = x;
        } else if (*str >= font->first && *str <= font->last) {
            GFXglyph *glyph = (GFXglyph *)&font->glyph[*str - font->first];
            v_draw_char_gfx(cursor_x, cursor_y, *str, font, color);
            cursor_x += glyph->xAdvance;
        }
        str++;
    }
}


// void v_draw_arc_hline(uint16_t cx, uint16_t cy,
//                       uint16_t inner_r, uint16_t outer_r,
//                       float start_angle_deg, float end_angle_deg,
//                       uint16_t color)
// {
//     if (!v_display_buffer) return;

//     // Normalize angles between 0–360
//     while (start_angle_deg < 0) start_angle_deg += 360;
//     while (end_angle_deg < 0) end_angle_deg += 360;
//     if (end_angle_deg < start_angle_deg) end_angle_deg += 360;

//     float start_rad = start_angle_deg * M_PI / 180.0f;
//     float end_rad = end_angle_deg * M_PI / 180.0f;

//     for (int r = inner_r; r <= outer_r; r++) {
//         float prev_y = -1000;
//         for (float angle = start_rad; angle <= end_rad; angle += 0.01f) {
//             float x = cosf(angle) * r;
//             float y = sinf(angle) * r;

//             int xi = (int)(cx + x);
//             int yi = (int)(cy + y);

//             // Draw horizontal span if on new Y row
//             if ((int)prev_y != yi) {
//                 int x_start = (int)(cx + cosf(angle - 0.02f) * r);
//                 int x_end   = (int)(cx + cosf(angle + 0.02f) * r);

//                 if (x_start > x_end) {
//                     int temp = x_start;
//                     x_start = x_end;
//                     x_end = temp;
//                 }

//                 for (int xh = x_start; xh <= x_end; xh++) {
//                     v_set_pixel(xh, yi, color, 1);
//                 }
//                 prev_y = yi;
//             }
//         }
//     }

//     gc9a01a_send_v_display_buffer(v_display_buffer);
// }



// void v_display_draw_arc_m(uint8_t cx, uint8_t cy, uint8_t radius_i, uint8_t radius_o, uint8_t start_a, uint8_t end_a, uint16_t color, bool filled) {
//     if (!v_display_buffer) {
//         ESP_LOGW(TAG, "v_display_buffer is NULL.");
//         return;
//     }

//     for(int deg = start_a; deg<end_a;deg++)
//     {
//         int x1 = cx+ radius_i * cos(deg * M_PI / 180.0);
//         int y1 = cy+ radius_i * sin(deg * M_PI / 180.0);
//         int x2 = cx+ radius_o * cos(deg * M_PI / 180.0);
//         int y2 = cy+ radius_o * sin(deg * M_PI / 180.0);
//         v_display_draw_line(x1,y1,x2,y2, color, 2);
//     }


//     gc9a01a_send_v_display_buffer(v_display_buffer);  // update display
// }

void v_display_draw_arc(int16_t cx, int16_t cy, int16_t r, int16_t thickness, float start_angle, float end_angle, uint16_t color) {
    for (float angle = start_angle; angle <= end_angle; angle += 0.5f) {
        float rad = angle * (M_PI / 180.0f);
        for (int t = 0; t < thickness; t++) {
            float tr = r - t;
            int16_t x = cx + tr * cosf(rad);
            int16_t y = cy + tr * sinf(rad);
            if((start_angle == angle ||end_angle == angle) && t==thickness/2)
            {
                v_display_draw_circle(x-1, y, thickness/2+1, color, true);
            }
            v_set_pixel(x, y, color, 2);
        }
    }
    
    // gc9a01a_send_v_display_buffer(v_display_buffer);
}

void v_draw_char_gfx_rotated(int16_t x, int16_t y, char c, const GFXfont *font, uint16_t color, float angle_deg) {
    if (c < font->first || c > font->last) return;

    const GFXglyph *glyph = &font->glyph[c - font->first];
    const uint8_t *bitmap = font->bitmap + glyph->bitmapOffset;

    uint8_t  w  = glyph->width;
    uint8_t  h  = glyph->height;
    int8_t   xo = glyph->xOffset;
    int8_t   yo = glyph->yOffset;

    // Convert angle to radians
    float angle_rad = angle_deg * (M_PI / 180.0f);
    float cos_a = cosf(angle_rad);
    float sin_a = sinf(angle_rad);

    // Character origin point before rotation (top-left of the char box)
    int16_t cx = x;
    int16_t cy = y;

    uint8_t bit = 0, bits = 0;

    for (uint8_t yy = 0; yy < h; yy++) {
        for (uint8_t xx = 0; xx < w; xx++) {
            if (!(bit--)) {
                bits = *bitmap++;
                bit = 7;
            }
            if (bits & 0x80) {
                // Coordinates relative to the glyph origin
                float rel_x = xo + xx + 0.5f;
                float rel_y = yo + yy + 0.5f;

                // Rotate around the original (x,y) point
                float xr = rel_x * cos_a - rel_y * sin_a;
                float yr = rel_x * sin_a + rel_y * cos_a;

                int16_t xi = (int16_t)roundf(cx + xr);
                int16_t yi = (int16_t)roundf(cy + yr);

                v_set_pixel(xi, yi, color, 1);
            }
            bits <<= 1;
        }
    }

    // gc9a01a_send_v_display_buffer(v_display_buffer);
}

void v_draw_text_gfx_rotated(int16_t x, int16_t y, const char *str, const GFXfont *font, uint16_t color, float angle_deg)
{
    float angle_rad = angle_deg * (M_PI / 180.0f);
    float cos_a = cosf(angle_rad);
    float sin_a = sinf(angle_rad);

    int16_t cursor_x = x;
    int16_t cursor_y = y;

    while (*str) {
        char c = *str++;
        if (c < font->first || c > font->last) continue;

        const GFXglyph *glyph = &font->glyph[c - font->first];

        // Draw the character
        v_draw_char_gfx_rotated(cursor_x, cursor_y, c, font, color, angle_deg);

        // Move to next character based on xAdvance
        float dx = glyph->xAdvance * cos_a;
        float dy = glyph->xAdvance * sin_a;

        cursor_x += dx;
        cursor_y += dy;
    }
}


void v_draw_bitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint16_t w, uint16_t h, uint16_t color) {
    if (!v_display_buffer) return;

    for (uint16_t j = 0; j < h; j++) {
        for (uint16_t i = 0; i < w; i++) {
            uint16_t byteIndex = (j * ((w + 7) / 8)) + (i / 8);
            uint8_t byte = bitmap[byteIndex];
            if (byte & (0x80 >> (i % 8))) {
                v_set_pixel(x + i, y + j, color, 1);
            }
        }
    }

    // Optional: flush buffer to screen
    // gc9a01a_send_v_display_buffer(v_display_buffer);
}

void v_draw_rgb565_image(int x, int y, int w, int h, const uint8_t *img_data) {
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            int pixel_index = (j * w + i) * 2;
            uint16_t color = (img_data[pixel_index] << 8) | img_data[pixel_index + 1];
            v_set_pixel(x + i, y + j, color, 1);
        }
    }

    // gc9a01a_send_v_display_buffer(v_display_buffer);
}

