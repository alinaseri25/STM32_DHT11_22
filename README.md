# STM32_DHT11_22

to use this lib you should be enable C99 Mode and Add --cpp to misc controls (in options for target --> C/C++ tab)

then you can add library to your project and use it

#include "dwt_stm32_delay.h"
#include "DHT22.hpp"


DHT22 dht22(GPIOx,GPIO_PIN_x,DHT_Type);
Temp_Hum temp_hum;




DWT_Delay_Init ();




temp_hum = dht22.DHT_Read();
float temp = temp_hum.Temp;
float hum = temp_hum.Hum;
