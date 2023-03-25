set(component_srcs 	"src/gui_main.c" "src/Watch.c"
                    "src/User_Touch.c" "src/Time.c"
                    "src/weather.c" "src/outswitch.c"
                    )

idf_component_register(SRCS "${component_srcs}"
                       INCLUDE_DIRS "include"
                       PRIV_INCLUDE_DIRS ""
                       PRIV_REQUIRES "FOUNT" 
                       "lvgl" "lvgl_esp32_drivers" 
                       "main" "WIFI"
                        REQUIRES ""
                       )
