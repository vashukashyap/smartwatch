#include <render.h>
#include <esp_heap_caps.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <gfx.h>
#include <gc9a01a.h>
#include <app_manager.h>


#define TAG "RENDER"

extern App *active_app;

extern bool DIRTY_RECT_ENABLE;
extern uint8_t* v_display_buffer;

bool init_render()
{
    gc9a01a_init_conn();
    gc9a01a_init();
    gc9a01a_mode(GC9A01A_NORMAL_MODE);
    ESP_LOGI(TAG, "render initialized");
    return true;
}

void render(render_function_t func, void *user_data, render_mode_t render_mode)
{
    if (!active_app || active_app->render_count >= MAX_RENDER_ENTRIES) return;

    ESP_LOGI(TAG, "render component added");

    active_app->render_stack[active_app->render_count++] = (render_stack_entry_t){
        .func = func,
        .userdata = user_data,
        .mode = render_mode,
    };
}



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
        vTaskDelay(pdMS_TO_TICKS(16)); // ~60fps
    }
}
