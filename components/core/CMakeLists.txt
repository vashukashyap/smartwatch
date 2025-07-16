idf_component_register(SRCS "app_constructor.c" "ui/ui.c" "gfx.c" "render.c" "fonts/FreeMonoBold24pt7b.c"
                    INCLUDE_DIRS "." "ui" "fonts" 
                    REQUIRES hal_drivers)