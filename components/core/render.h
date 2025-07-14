#pragma once

#include <stdbool.h>



#define MAX_RENDER_ENTRIES 6


typedef void (*render_function_t)(void *user_data);


typedef enum {
    NONE,
    ONCE,
    CONTINOUS
} render_mode_t;


typedef struct {
    render_function_t func;
    void *userdata;
    render_mode_t mode;
} render_stack_entry_t;



typedef struct {
    render_stack_entry_t entries[MAX_RENDER_ENTRIES]; // say 4 or 8
    int render_count;
} render_app_t;


bool init_render();

void render(render_function_t func, void *user_data, render_mode_t render_mode);

void render_remove(render_function_t func);

void render_execute();
