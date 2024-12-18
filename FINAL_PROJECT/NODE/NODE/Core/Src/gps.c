

#include "gps.h"

float getLatGps(uint8_t* dataGps)
{
	char copyDataGps[80];
	char str_lat[10];
	char hourLat[3], minLat[8];
	float latGps;
	char* ptr = strstr((char *)dataGps, "GPRMC");
	if (ptr != NULL) {
		int posDataGps = ptr - (char *)dataGps;
		int flagComma = 0, flag = 0;
		int i;
		for (i = 0; i<80 && dataGps[posDataGps + i] != '\0'; ++i) {
			copyDataGps[i] = dataGps[posDataGps + i];
		}
		copyDataGps[i] = '\0';
		while (flagComma != 2) {
			if (copyDataGps[flag] == ',') {
				flagComma += 1;
			}
			flag += 1;
		}
		//flagComma = 2, behind 'A'
		if (copyDataGps[flag] == 'A') {
			while (flagComma != 3) {
				if (copyDataGps[flag] == ',') {
					flagComma += 1;
				}
				flag += 1;
			}
			strncpy(str_lat, copyDataGps + flag, sizeof(str_lat));
			str_lat[9]='\0';
			strncpy(hourLat, str_lat, 2);
			hourLat[2]='\0';
			strncpy(minLat, str_lat+2, 7);
			minLat[7]='\0';
			latGps = atof(hourLat) + (atof(minLat)/60);
			return latGps;
		}
		else{return 0;}
	}
	else{return 0;}
}



float getLongGps(uint8_t* dataGps)
{
	char copyDataGps[80];
	char str_long[11];
	char hourLong[4], minLong[8];
	float longGps;
	char* ptr = strstr((char *)dataGps, "GPRMC");
	if (ptr != NULL) {
		int posDataGps = ptr - (char *)dataGps;
		int flagComma = 0, flag = 0;
		int i;
		for (i = 0; i<80 && dataGps[posDataGps + i] != '\0'; ++i) {
			copyDataGps[i] = dataGps[posDataGps + i];
		}
		copyDataGps[i] = '\0';
		while (flagComma != 2) {
			if (copyDataGps[flag] == ',') {
				flagComma += 1;
			}
			flag += 1;
		}
		//flagComma = 2, behind 'A'
		if (copyDataGps[flag] == 'A') {
			while (flagComma != 5) {
				if (copyDataGps[flag] == ',') {
					flagComma += 1;
				}
				flag += 1;
			}
			strncpy(str_long, copyDataGps + flag, sizeof(str_long));
			str_long[10]='\0';
			strncpy(hourLong, str_long, 3);
			hourLong[3]='\0';
			strncpy(minLong, str_long + 3, 7);
			minLong[7]='\0';
			longGps = atof(hourLong) + (atof(minLong)/60);
			return longGps;
		}
		else{return 0;}
	}
	else{return 0;}
}
