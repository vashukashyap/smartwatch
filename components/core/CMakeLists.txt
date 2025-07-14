idf_component_register(SRCS "ui/ui.c" "gfx.c" "render.c" "fonts/FreeMonoBold24pt7b.c" "app_manager.c"
                    INCLUDE_DIRS "." "ui" "fonts" 
                    REQUIRES hal_drivers)