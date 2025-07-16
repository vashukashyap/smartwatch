#include <render.h>
#include <esp_heap_caps.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <gfx.h>
#include <gc9a01a.h>
#include <app_constructor.h>
#include <string.h>


#define TAG "RENDER"

extern App *active_app;
extern bool DIRTY_RECT_ENABLE;
extern uint8_t* v_display_buffer;


/*
    @brief initalized the render and the display.
    @param void
    @return void
*/
bool render_init()
{
    gc9a01a_init_conn();
    gc9a01a_init();
    gc9a01a_mode(GC9A01A_NORMAL_MODE);
    ESP_LOGI(TAG, "render initialized");
    return true;
}


/*
    @brief add the components of the app in the render stack for rendering
    @param  func
                render function pointer
    @param  user_data
                user_data pointer
    @param  render_mode
                render mode for the type of rendering of the component. ( 'NONE' , 'ONCE' , 'CONTINOUS' )
    @return void
*/
void render_add_component(render_function_t func, void *user_data, render_mode_t render_mode)
{
    if (!active_app || active_app->render_count >= MAX_RENDER_ENTRIES) return;

    ESP_LOGI(TAG, "render component added");

    active_app->render_stack[active_app->render_count++] = (render_stack_entry_t){
        .func = func,
        .userdata = user_data,
        .mode = render_mode,
    };
}

/*
    @brief  loop continuously in the render stack
    @return void
*/
void render_execute()
{
    ESP_LOGI(TAG, "render component execute");
    while (true) {
        if (!active_app || active_app->render_count == 0) {
            ESP_LOGI(TAG, "render stack empty");
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }
        for (int i = 0; i < active_app->render_count; i++) {
            render_stack_entry_t *entry = &active_app->render_stack[i];

            if(entry->mode == NONE){
                DIRTY_RECT_ENABLE = false;
                entry->func(entry->userdata);
            }

            if(entry->mode == ONCE)
            {
                DIRTY_RECT_ENABLE = true;
                entry->func(entry->userdata);
                entry->mode = NONE;
            }

            if(entry->mode == CONTINOUS)
            {
                DIRTY_RECT_ENABLE = true;
                entry->func(entry->userdata);
            }
        }

        flush_dirty_rects();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
