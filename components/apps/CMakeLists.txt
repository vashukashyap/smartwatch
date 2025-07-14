idf_component_register(SRCS "count.c" "stopwatch.c"
                    INCLUDE_DIRS "."
                    REQUIRES hal_drivers core)