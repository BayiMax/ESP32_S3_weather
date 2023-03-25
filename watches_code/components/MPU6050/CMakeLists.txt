set(component_srcs 	"src/mpu6050.c")

idf_component_register(SRCS "${component_srcs}"
                       INCLUDE_DIRS "include"
                       PRIV_INCLUDE_DIRS ""
                       PRIV_REQUIRES "driver" "main" "User_I2C"
                       REQUIRES "")
