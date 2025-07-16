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
    v_display_draw_rectangle(0,0, GC9A01A_TFTWIDTH, GC9A01A_TFTHEIGHT, GC9A01A_RED);
}

void counting(void *args)
{
    values* count_values = (values*) args;
    v_draw_text_gfx(120,132,count_values->time, &FreeMonoBold24pt7b, GC9A01A_WHITE);
}


void count()
{
    values *count_values = (values*) malloc(sizeof(values));

    render_add_component(count_background, NULL, ONCE);
    render_add_component(counting, count_values, CONTINOUS);

        
    for(int i=0; i<10; i++)
    {
        sprintf(count_values->time, "%d", i);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    app_constructor_remove(&count_app);
}


