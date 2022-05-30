# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS 
    "main.cpp" 
    "system.cpp"
    "network.c"
    "webserver.c"
    "webdata.cpp"
    "xfifo.c"
    "common/maths.c"
    "driver/led/drv_led.c" 
    "driver/pwm/drv_pwm.c"
    "driver/i2c/drv_i2c.c"
    "sensors/sensors_common.c"
    "sensors/mag_hmc5883.c"
    "sensors/mag.cpp"
    
)

set(COMPONENT_ADD_INCLUDEDIRS 
    "./" 
    "common"
    "./driver/led" 
    "./driver/pwm"
    "./driver/i2c"
    "./sensors"
    "./library"
)


register_component()
