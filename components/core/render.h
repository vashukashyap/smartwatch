#pragma once

#include <stdbool.h>

typedef void (*render_function_t)(void *user_data);


typedef enum {
    NONE,
    ONCE,
    CONTINOUS
} render_t;


typedef struct
{
    render_function_t func;
    void *userdata;
    render_t render_type;
} render_component_t;


bool init_render();

void render(render_function_t func, void *user_data, render_t render_type);

void render_execute();
