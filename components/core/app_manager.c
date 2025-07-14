#include <app_manager.h>
#include <stdio.h>
#include <string.h>
#include <esp_log.h>


#define TAG "App Manager"


App *active_app = NULL;



void launch_app(App *app)
{
    if (!app) return;

    // Clear previous app render stack
    if (active_app) active_app->render_count = 0;

    active_app = app;
    active_app->render_count = 0;

    app->entry_point(); // This will populate render_stack via render()
}


void app_remove_with_free(App *app) {
    if (!app) return;
    
    ESP_LOGI(TAG, "CLosing \'%s\' with memory cleanup", app->name);
    for (int i = 0; i < app->render_count; i++) {
        if (app->render_stack[i].userdata) {
            free(app->render_stack[i].userdata);
            app->render_stack[i].userdata = NULL;
        }
    }

    memset(app->render_stack, 0, sizeof(render_stack_entry_t) * MAX_RENDER_ENTRIES);
    app->render_count = 0;

    if (active_app == app) {
        active_app = NULL;
    }
     ESP_LOGI(TAG, "Closed");
}
