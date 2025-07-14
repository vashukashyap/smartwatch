#include <stdio.h>
#include <freertos/FreeRTOS.h>
// #include <gc9a01a.h>
#include <gfx.h>
#include <ui.h>
#include <render.h>
// #include <fonts/FreeMonoBold24pt7b.h>
#include <stopwatch.h>
#include <count.c>
#include <app_manager.h>

// extern uint8_t* v_display_buffer; 

void app_main(void)
{  

    init_render();
    xTaskCreate(render_execute, "rendering", (1024)*2, NULL, 5, NULL);



    launch_app(&stopwatch_app);
    launch_app(&count_app);
    
}