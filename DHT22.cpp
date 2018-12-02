#include "DHT22.hpp"


DHT22::DHT22(GPIO_TypeDef *_GPIOx,uint16_t _GPIO_Pin,uint8_t _DHTType)
{
	DWT_Delay_Init();
	GPIOx = _GPIOx;
	GPIO_Pin = _GPIO_Pin;
	DHT22State = HAL_OK;
	DHTType = _DHTType;
	lasttime = 0;
}

void DHT22::set_gpio_output(void)
{
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx,&GPIO_InitStruct);
}
	
void DHT22::set_gpio_input(void)
{
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOx,&GPIO_InitStruct);
}

HAL_StatusTypeDef DHT22::begin(void)
{
	DHT22State = HAL_ERROR;
	if(DHTType == DHT2X)
	{
		set_gpio_output ();  // set the pin as output
		HAL_GPIO_WritePin(GPIOx,GPIO_Pin,GPIO_PIN_RESET);  // pull the pin low
		DWT_Delay_us (500);   // wait for 500us
		HAL_GPIO_WritePin(GPIOx,GPIO_Pin,GPIO_PIN_SET);   // pull the pin high
		DWT_Delay_us (30);   // wait for 30us
		set_gpio_input ();   // set as input
		DHT22State = HAL_OK;
	}
	else if(DHTType == DHT1X)
	{
		set_gpio_output ();  // set the pin as output
		HAL_GPIO_WritePin (GPIOx,GPIO_Pin,GPIO_PIN_RESET);   // pull the pin low
		DWT_Delay_us (18000);   // wait for 18ms
		set_gpio_input ();   // set as input
		DHT22State = HAL_OK;
	}
	return HAL_OK;
}

HAL_StatusTypeDef DHT22::check_response(void)
{
	bool check = false;
	if(DHTType == DHT2X)
	{
		DWT_Delay_us (40);
		if (!(HAL_GPIO_ReadPin(GPIOx, GPIO_Pin)))
		{
			DWT_Delay_us(80);
			if ((HAL_GPIO_ReadPin(GPIOx, GPIO_Pin))) check = true;
		}
		else
		{
			DHT22State = HAL_TIMEOUT;
			return HAL_TIMEOUT;
		}
		while ((HAL_GPIO_ReadPin(GPIOx, GPIO_Pin)));   // wait for the pin to go low
	}
	else if(DHTType == DHT1X)
	{
		DWT_Delay_us (40);
		if (!(HAL_GPIO_ReadPin(GPIOx, GPIO_Pin)))
		{
			DWT_Delay_us (80);
			if ((HAL_GPIO_ReadPin(GPIOx, GPIO_Pin))) check = 1;
		}
		else
		{
			DHT22State = HAL_TIMEOUT;
			return HAL_TIMEOUT;
		}
		while ((HAL_GPIO_ReadPin(GPIOx, GPIO_Pin)));   // wait for the pin to go low
	}
	if(check)
	{
		DHT22State = HAL_OK;
		return HAL_OK;
	}
	else
	{
		DHT22State = HAL_TIMEOUT;
		return HAL_TIMEOUT;
	}
}

float DHT22::readTemperature(bool S, bool force)
{
	DHT_Read();
	return DHT_Temp_Hum.Temp;
}

float DHT22::convertCtoF(float c)
{
	return c * 1.8 + 32;
}

float DHT22::convertFtoC(float f)
{
	return (f - 32) * 0.55555;
}

float DHT22::readHumidity(bool force)
{
	DHT_Read();
	return DHT_Temp_Hum.Hum;
}

uint8_t DHT22::read(bool force)
{
	uint8_t i,j;
	for (j=0;j<8;j++)
	{
		while (!(HAL_GPIO_ReadPin (GPIOx, GPIO_Pin)));   // wait for the pin to go high
		DWT_Delay_us (40);   // wait for 40 us
		if ((HAL_GPIO_ReadPin (GPIOx, GPIO_Pin)) == 0)   // if the pin is low 
		{
			i&= ~(1<<(7-j));   // write 0
		}
		else i|= (1<<(7-j));  // if the pin is high, write 1
		while ((HAL_GPIO_ReadPin (GPIOx, GPIO_Pin)));  // wait for the pin to go low
	}
	return i;
}

Temp_Hum DHT22::DHT_Read(void)
{
	if(((HAL_GetTick() - DHTREADTIME) <= lasttime))
	{
		return DHT_Temp_Hum;
	}
	uint8_t Rh_byte1,Rh_byte2,Temp_byte1,Temp_byte2,sum;
	uint16_t TEMP,RH;
	float HMD,TMP;
	begin();
	
	if(check_response() != HAL_OK)
	{
		DHT_Temp_Hum.Temp = DHT_Temp_Hum.Hum = 0;
		return DHT_Temp_Hum;
	}
	
	DHT22State = HAL_OK;
	
	Rh_byte1 = read();
	Rh_byte2 = read();
	
	Temp_byte1 = read();
	Temp_byte2 = read();
	
	sum = read();
	if (sum!=(Rh_byte1+Rh_byte2+Temp_byte1+Temp_byte2))    // if the data is correct
	{
		DHT22State = HAL_BUSY;
	}
	
	if(DHTType == DHT2X)
	{
		TEMP = ((Temp_byte1<<8)|Temp_byte2);
		RH = ((Rh_byte1<<8)|Rh_byte2);
		
		HMD = RH;
		HMD /= 10;
		
		TMP = TEMP;
		TMP /= 10;
	}
	else if(DHTType == DHT1X)
	{
		TEMP = Temp_byte1;
		RH = Rh_byte1;
		
		HMD = RH;
		
		TMP = TEMP;
	}
	
	if(TMP > 100 || TMP < -50)
	{
		DHT22State = HAL_ERROR;
	}
	else
	{
		DHT_Temp_Hum.Temp = TMP;
	}
	if(HMD > 100 || HMD < .1)
	{
		DHT22State = HAL_ERROR;
	}
	else
	{
		DHT_Temp_Hum.Hum = HMD;
	}
	return DHT_Temp_Hum;
}

HAL_StatusTypeDef DHT22::DHTStatus(void)
{
	return DHT22State;
}

uint8_t DHT22::GetDHTType(void)
{
	return DHTType;
}

void DHT22::SetDHTType(uint8_t _DHTType)
{
	DHTType = _DHTType;
}
