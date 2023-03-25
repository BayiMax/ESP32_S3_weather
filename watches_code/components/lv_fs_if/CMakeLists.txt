set(component_srcs 	"src/lv_fs_fatfs.c"
"src/lv_fs_if.c" 
"src/lv_fs_pc.c")

idf_component_register(SRCS "${component_srcs}"
                       INCLUDE_DIRS "include"
                       PRIV_INCLUDE_DIRS ""
                       PRIV_REQUIRES "driver" "main" "SDCARD"
                       REQUIRES 
                       lvgl
                       SDCARD
                       fatfs)
