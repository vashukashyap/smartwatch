idf_component_register(SRCS "ui/ui.c" "display.c" "render.c"
                    INCLUDE_DIRS "." "ui" "fonts" 
                    REQUIRES hal_drivers)