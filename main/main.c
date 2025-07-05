#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <gc9a01a.h>



void app_main(void)
{  
   
    gc9a01a_init_conn();
    gc9a01a_init();
    gc9a01a_mode(GC9A01A_NORMAL_MODE);
    gc9a01a_draw_screen(0,239,0,239, GC9A01A_PINK);
    // fill_color(GC9A01A_PINK);
    vTaskDelay(pdMS_TO_TICKS(3000));

    gc9a01a_draw_screen(100,239,100,239, GC9A01A_CYAN);
    // fill_color(GC9A01A_CYAN);
    vTaskDelay(pdMS_TO_TICKS(3000));

    gc9a10a_sleep(GC9A01A_SLEEP_ON);
    gc9a01a_draw_screen(100,200,100,200, GC9A01A_RED);
    // fill_color(GC9A01A_RED);
    vTaskDelay(pdMS_TO_TICKS(3000));

    
    gc9a01a_draw_screen(10,220,10,220, GC9A01A_NAVY);

    gc9a10a_sleep(GC9A01A_SLEEP_OFF);
    gc9a01a_draw_screen_pixel(100, 100, GC9A01A_BLACK);
    gc9a10a_inversion(GC9A01A_INVERSION_ENABLE);

    vTaskDelay(pdMS_TO_TICKS(3000));
    gc9a01a_draw_screen(0,240,0,240, GC9A01A_WHITE);
   
    // vTaskDelay(pdMS_TO_TICKS(3000));
    // gc9a01a_draw_screen(0,240,0,240, GC9A01A_BLACK);

    vTaskDelay(pdMS_TO_TICKS(3000));


     vTaskDelay(pdMS_TO_TICKS(3000));

    gc9a10a_inversion(GC9A01A_INVERSION_DISABLE);

}