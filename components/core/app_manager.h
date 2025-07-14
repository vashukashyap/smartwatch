#pragma once
#include <render.h>

typedef struct {
    const char *name;
    void (*entry_point)(void); // app start function
    render_stack_entry_t render_stack[MAX_RENDER_ENTRIES];
    int render_count;
} App;



void launch_app(App *app);

void app_remove_with_free(App *app);