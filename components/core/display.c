#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <gc9a01a.h>


#define real_radius(radius) (120 - radius)


void display_draw_rectangle(uint16_t degree, uint16_t radius, uint16_t width, uint16_t height)
{
  uint16_t *rect = (uint16_t*)  malloc(sizeof(uint16_t) * width * height);
  
  if (rect == NULL) 
  {
    printf("ubale to create buffer");
    return;
  }

  uint16_t set_color = (GC9A01A_RED >> 8) | (GC9A01A_RED <<8);
  for(int i=0; i < (height * width); i++)
  {
      rect[i] = set_color;
  }

  uint16_t pos_x = real_radius(radius) * cos((double)real_radius(radius));
  uint16_t pos_y = real_radius(radius) * sin((double)real_radius(radius));

  gc9a01a_draw_cursor_set(pos_x, pos_x+width, pos_y, pos_y+width);
  gc9a10a_display_buffer(rect, width*height);
  
}

