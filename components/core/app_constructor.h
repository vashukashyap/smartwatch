#pragma once
#include <render.h>


// App constructor structure
typedef struct {
    const char *name;
    void (*entry_point)(void); // app start function
    render_stack_entry_t render_stack[MAX_RENDER_ENTRIES];
    int render_count;
} App;



void app_constructor_launch(App *app);

void app_constructor_remove(App *app);