#include <stdio.h>
#include <display.h>
#include <gc9a01a.h>
#include <fonts/FreeMonoBold24pt7b.h>
#include <freertos/FreeRTOS.h>
#include <malloc.h>
#include <render.h>

extern uint8_t* v_display_buffer; 

typedef struct
{
    float angle;
    char time[5];
} values;

void background()
{
    // printf("CALLED BG\n");
    v_display_draw_rectangle(0,0, GC9A01A_TFTWIDTH, GC9A01A_TFTHEIGHT, GC9A01A_WHITE);
}

void progressbar(void *args)
{
    values* stopwatch_values = (values*) args;
    // printf("CALLED PROG %f\n", stopwatch_values->angle);
    v_draw_text_gfx(120,132,stopwatch_values->time, &FreeMonoBold24pt7b, GC9A01A_BLACK);
    v_display_draw_arc(120,120, 100, 10, 0, stopwatch_values->angle,GC9A01A_BLUE);
}


void stopwatch()
{
    values *stopwatch_values = (values*) malloc(sizeof(values));
    // stopwatch_values->angle = 30;
    
    // printf("IN STOPWWATCH \n");
    render(background, NULL, ONCE);
    render(progressbar, stopwatch_values, CONTINOUS);

        
    for(int i=0; i<100; i++)
    {
        sprintf(stopwatch_values->time, "%d", i);
        stopwatch_values->angle = i*3.6;
        vTaskDelay(pdMS_TO_TICKS(1000));
        // gc9a01a_send_v_display_buffer(v_display_buffer);
    }

}

