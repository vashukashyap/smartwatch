#include <render.h>
#include <esp_heap_caps.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <stdbool.h>
#include <display.h>
#include <gc9a01a.h>

#define MAX_RENDER_COMPONENTS 64

extern bool DIRTY_RECT_ENABLE;

static render_component_t *render_stack = NULL;
static int render_stack_count = 0;

extern uint8_t* v_display_buffer;

bool init_render()
{
    render_stack = heap_caps_malloc(sizeof(render_component_t) * MAX_RENDER_COMPONENTS, MALLOC_CAP_DMA);
    if(render_stack==NULL)
    {
        ESP_LOGW("RENDER", "enable to create render stack");
        return false;
    }
    render_stack_count = 0;
        gc9a01a_init_conn();
    gc9a01a_init();
    gc9a01a_mode(GC9A01A_NORMAL_MODE);
    ESP_LOGI("RENDER", "render stack initalized");
    return true;
}


void render(render_function_t func, void *user_data, render_t render_type)
{
    if(render_stack_count >= MAX_RENDER_COMPONENTS || !render_stack)
    {
        ESP_LOGW("RENDER", "enable to create render stack");
        return;
    }

    render_stack[render_stack_count++] = (render_component_t){func, user_data, render_type};
    ESP_LOGI("RENDER", "component added");
}

void render_execute()
{
    // if(!render_stack) return;
    ESP_LOGI("RENDER", "render executing");
    int loop_index = 0;
    while (true) {
        if (!render_stack || render_stack_count == 0) {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        
        render_function_t f = render_stack[loop_index].func;
        void *data = render_stack[loop_index].userdata;

        if(render_stack[loop_index].render_type == NONE)
        {
            DIRTY_RECT_ENABLE = false;
            f(data);
        }

        if(render_stack[loop_index].render_type == ONCE)
        {
            f(data);
            render_stack[loop_index].render_type = NONE;
        }
        
        if (render_stack[loop_index].render_type == CONTINOUS) {
            DIRTY_RECT_ENABLE = true;
            f(data);
        }
        
        flush_dirty_rects();
        taskYIELD();
        // gc9a01a_send_v_display_buffer(v_display_buffer);
        // vTaskDelay(pdMS_TO_TICKS(10));

        loop_index++;
        if (loop_index >= render_stack_count) loop_index = 0;
    }
}