
#include "sensor.h"
extern I2C_HandleTypeDef hi2c1;
extern ADC_HandleTypeDef hadc1;
uint8_t CMD_MEASURE_TEMP_HUM[2] = {0x2C, 0x06};

void SHT31_ReadTempHumidity(float* temp, float* humidity)
{
    uint8_t data[6];
    uint16_t temp_raw, humidity_raw;

    HAL_I2C_Master_Transmit(&hi2c1, SHT31_ADDR, CMD_MEASURE_TEMP_HUM, 2, 500);
    HAL_Delay(200);
    HAL_I2C_Master_Receive(&hi2c1, SHT31_ADDR, data, 6, 1000);
    HAL_Delay(200);
    temp_raw = data[0] << 8 | data[1];
    *temp = ((float)temp_raw * 175.0f / 65535.0f) - 45.0f;
    humidity_raw = data[3] << 8 | data[4];
    *humidity = ((float)humidity_raw * 100.0f / 65535.0f);
    HAL_Delay(200);
}



int getPM25(uint8_t* data_rx, uint8_t* copy_data_rx, int size){
	  int flag = 0;
	  int pm2_5;
	  memcpy(copy_data_rx,data_rx,size);
	  while(1){
		  int sumOfDataFrame=0;
		  if(flag+32<64){
			  if((copy_data_rx[flag]==66) && (copy_data_rx[flag+1]==77)){
				  if((copy_data_rx[flag+2]==0) && (copy_data_rx[flag+3]==28)){
					  for(int i=flag;i<flag+30;i++){
						  sumOfDataFrame+=copy_data_rx[i];
					  }
					  if(sumOfDataFrame == ((copy_data_rx[flag+30]<<8) + copy_data_rx[flag+31])){
						  pm2_5 = (copy_data_rx[flag+6]<<8) + copy_data_rx[flag+7];
						  //sprintf(str,"%d",pm2_5);
						  break;
	 				  }
					  else{
						  flag+=4;
					  }
				  }
				  else{
					  flag+=2;
				  }
			  }
			  else{
				  flag+=1;
			  }
		  }
		  else{
			  flag=0;
			  memcpy(copy_data_rx,data_rx,size);
		  }
	  }
	  return pm2_5;
}



double getCOValue(){
	uint16_t var;
	double vrl, rs, ratio, ppm;
	double r0 = 355.27;
	HAL_ADC_Start(&hadc1);
	HAL_Delay(100);
	var=HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);
	vrl = (var * 5.0) / 4095.0;
	rs = ((5 * 1000) / vrl) - 1000;
	ratio = rs / r0;
	ppm = pow(10, ((log10(ratio) - 1.3055) / -0.6528));
	return ppm;
}
