#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <gfx.h>
#include <ui.h>
#include <render.h>
#include <stopwatch.h>
#include <count.c>
#include <app_constructor.h>


void app_main(void)
{  

    render_init();
    xTaskCreate(render_execute, "rendering", (1024)*2, NULL, 5, NULL);

    app_constructor_launch(&stopwatch_app);
    app_constructor_launch(&count_app);
    
}