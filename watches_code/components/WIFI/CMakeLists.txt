set(component_srcs 	"src/User_Wifi.c" "src/web_serve.c")

idf_component_register(SRCS "${component_srcs}"
                       INCLUDE_DIRS "include"
                       PRIV_INCLUDE_DIRS ""
                      #  EMBED_TXTFILES "certs/cacert.pem"
                      #                 "certs/prvtkey.pem"
                       PRIV_REQUIRES "driver" "main"
                       REQUIRES "nvs_flash")
