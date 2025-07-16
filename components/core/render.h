#pragma once

#include <stdbool.h>

// max render stack components the can be added in the app's render stack
#define MAX_RENDER_ENTRIES 6

//  render function type pointer
typedef void (*render_function_t)(void *user_data);

//  rendering types
typedef enum {
    NONE,
    ONCE,
    CONTINOUS
} render_mode_t;

// render component structure
typedef struct {
    render_function_t func;
    void *userdata;
    render_mode_t mode;
} render_stack_entry_t;

// render components array
typedef struct {
    render_stack_entry_t entries[MAX_RENDER_ENTRIES]; // say 4 or 8
    int render_count;
} render_app_t;


bool render_init();

void render_add_component(render_function_t func, void *user_data, render_mode_t render_mode);

void render_execute();
