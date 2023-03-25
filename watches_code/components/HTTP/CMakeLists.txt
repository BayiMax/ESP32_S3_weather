set(component_srcs 	"src/Http_Weather.c"
					          "src/Http_time.c")

idf_component_register(SRCS "${component_srcs}"
                       INCLUDE_DIRS "include"
                       PRIV_INCLUDE_DIRS ""
                       PRIV_REQUIRES "driver" "main"  #指定对其他自定义组件的依赖
                       REQUIRES "esp-tls" "spi_flash" "esp_http_client" "json" "WIFI" #该参数指定对系统组件的依赖
                       )
