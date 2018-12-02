#ifndef DHT22CPP_H
#define DHT22CPP_H

#define DHTREADTIME 1000

//Define DHT Type
#define DHT2X 0x0 //for DHT21 & DHT22
#define DHT1X 0x1 //for DHT11

#include "stm32f1xx_hal.h"
#include "dwt_stm32_delay.h"

extern IWDG_HandleTypeDef hiwdg;

struct Temp_Hum
{
	float Temp;
	float Hum;
};

class DHT22
{
public:
	DHT22(GPIO_TypeDef *_GPIOx,uint16_t _GPIO_Pin,uint8_t _DHTType = DHT2X);
	Temp_Hum DHT_Read(void);
	float readTemperature(bool S=false, bool force=false);
	float readHumidity(bool force=false);
	HAL_StatusTypeDef DHTStatus(void);
	uint8_t GetDHTType(void);
	void SetDHTType(uint8_t _DHTType);
private:
	
	HAL_StatusTypeDef begin(void);
	HAL_StatusTypeDef check_response(void);
	
	void set_gpio_output(void);
	void set_gpio_input(void);
	uint8_t read(bool force=false);

	float convertCtoF(float c);
  float convertFtoC(float f);

	
	GPIO_InitTypeDef GPIO_InitStruct;
	uint16_t GPIO_Pin;
	GPIO_TypeDef *GPIOx;
	HAL_StatusTypeDef DHT22State;
	Temp_Hum DHT_Temp_Hum;
	uint32_t lasttime;
	uint8_t DHTType;

};

#endif
