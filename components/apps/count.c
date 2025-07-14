#include <stdio.h>
#include <gfx.h>
#include <gc9a01a.h>
#include <fonts/FreeMonoBold24pt7b.h>
#include <freertos/FreeRTOS.h>
#include <malloc.h>
#include <render.h>
#include <count.h>


App count_app = {
    .name = "count",
    .entry_point = count
};


typedef struct
{
    char time[5];
} values;

void count_background()
{
    // printf("CALLED BG\n");
    v_display_draw_rectangle(0,0, GC9A01A_TFTWIDTH, GC9A01A_TFTHEIGHT, GC9A01A_RED);
}

void counting(void *args)
{
    values* count_values = (values*) args;
    // printf("CALLED PROG %f\n", count_values->angle);
    v_draw_text_gfx(120,132,count_values->time, &FreeMonoBold24pt7b, GC9A01A_WHITE);
    // v_display_draw_arc(120,120, 100, 10, 0, count_values->angle,GC9A01A_BLUE);
}


void count()
{
    values *count_values = (values*) malloc(sizeof(values));
    // count_values->angle = 30;
    
    // printf("IN STOPWWATCH \n");
    render(count_background, NULL, ONCE);
    render(counting, count_values, CONTINOUS);

        
    for(int i=0; i<10; i++)
    {
        sprintf(count_values->time, "%d", i);
        vTaskDelay(pdMS_TO_TICKS(1000));
        // gc9a01a_send_v_display_buffer(v_display_buffer);
    }
    app_remove_with_free(&count_app);
}


