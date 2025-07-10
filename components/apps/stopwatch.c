#include <stdio.h>
#include <display.h>
#include <gc9a01a.h>
#include <fonts/FreeMonoBold24pt7b.h>
#include <freertos/FreeRTOS.h>

extern uint8_t* v_display_buffer; 

void background()
{
    v_display_draw_rectangle(0,0, GC9A01A_TFTWIDTH, GC9A01A_TFTHEIGHT, GC9A01A_BLACK);
}

void progressbar(char *value,float angle)
{
    v_display_draw_arc(120,120, 100, 10, 0,angle,GC9A01A_BLUE);
    v_draw_text_gfx(90,132,value, &FreeMonoBold24pt7b, GC9A01A_WHITE);
}


void stopwatch()
{
    char time[5];
    
    for(int i=0; i<100; i++)
    {
        sprintf(time, "%d", i);
        float angle = i*3.6;
        background();
        progressbar(time, angle);
        vTaskDelay(pdMS_TO_TICKS(1000));
        gc9a01a_send_v_display_buffer(v_display_buffer);
    }
}

