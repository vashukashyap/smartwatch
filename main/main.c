#include <stdio.h>
#include <freertos/FreeRTOS.h>
// #include <gc9a01a.h>
#include <display.h>
#include <ui.h>
#include <render.h>
// #include <fonts/FreeMonoBold24pt7b.h>
#include <stopwatch.h>

// extern uint8_t* v_display_buffer; 

void app_main(void)
{  
   

    // for(int i=30; i<=150; i++)
    // {
    //     v_display_draw_rectangle(0,0,240,240,GC9A01A_RED);
    //     v_draw_text_gfx(50,90, "16:12", &FreeMonoBold24pt7b, GC9A01A_WHITE);
    //     v_display_draw_arc(120,120, 100, 10, 30, i, GC9A01A_YELLOW);
    //     // vTaskDelay(pdMS_TO_TICKS(10));
    //     gc9a01a_send_v_display_buffer(v_display_buffer);
    // }
    // for(int i=150; i>=30; i--)
    // {
    //     v_display_draw_rectangle(0,0,240,240,GC9A01A_RED);
    //     v_draw_text_gfx(50,90, "16:12", &FreeMonoBold24pt7b, GC9A01A_WHITE);
    //     if(i>30){
    //         v_display_draw_arc(120,120, 100, 10, 30, i, GC9A01A_YELLOW);
    //     }
    //     // vTaskDelay(pdMS_TO_TICKS(10));
    //     gc9a01a_send_v_display_buffer(v_display_buffer);
    // }
    init_render();
    xTaskCreate(render_execute, "rendering", (1024)*2, NULL, 5, NULL);
    stopwatch();
    // v_display_draw_arc(120,120, 88, 10, 30, 120, GC9A01A_GREEN);
    // v_display_draw_arc(120,120, 78, 10, 30, 90, GC9A01A_BLUE);
}