

#ifndef INC_SENSOR_H_
#define INC_SENSOR_H_

#endif /* INC_SENSOR_H_ */

#include "main.h"
#include<math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define SHT31_ADDR (0x44 << 1) // SHT31 I2C address shifted left by 1 bit

void SHT31_ReadTempHumidity(float* temp, float* humidity);
int getPM25(uint8_t* data_rx, uint8_t* copy_data_rx, int size);
double getCOValue();

