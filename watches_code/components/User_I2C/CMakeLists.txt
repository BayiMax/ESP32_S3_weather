set(component_srcs 	"src/User_I2C.c")

idf_component_register(SRCS "${component_srcs}"
                       INCLUDE_DIRS "include"
                       PRIV_INCLUDE_DIRS ""
                       PRIV_REQUIRES "driver" "main" 
                       REQUIRES "")
