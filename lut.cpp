#include <Husarnet.h>

#define LUT_SIZE 60 //all defined LUTs should be this size

uint8_t getLutVal(String type)
{
	static uint8_t cnt = 0;
	
	static const uint8_t sineWaveLUT[] = { 128, 141, 154, 167, 179, 191, 202, 213, 222, 231, 238, 244, 249, 252, 254, 255, 254, 252, 249, 244,
                               238, 231, 222, 213, 202, 191, 179, 167, 154, 141, 128, 114, 101, 88, 76, 64, 53, 42, 33, 24,
                               17, 11, 6, 3, 1, 0, 1, 3, 6, 11, 17, 24, 33, 42, 53, 64, 76, 88, 101, 114
                             }; //60
	static const uint8_t triangleWaveLUT[] = { 9, 17, 26, 34, 43, 51, 60, 68, 77, 85, 94, 102, 111, 119, 128, 136, 145, 153, 162, 170,
                                   179, 187, 196, 204, 213, 221, 230, 238, 247, 255, 247, 238, 230, 221, 213, 204, 196, 187, 179, 170,
                                   162, 153, 145, 136, 128, 119, 111, 102, 94, 85, 77, 68, 60, 51, 43, 34, 26, 17, 9, 0
                                 }; //60
	static const uint8_t squareWaveLUT[] = { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                                 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                               };
	cnt ++;
	if(cnt>LUT_SIZE-1) {
		cnt = 0;
	}
	
	if(type == "sine") {
		return sineWaveLUT[cnt];
	}
	
	if(type == "triangle") {
		return triangleWaveLUT[cnt];
	}
	
	if(type == "square") {
		return squareWaveLUT[cnt];
	}
	
	if(type == "none") {
		return 0;
	}
	
	return 127;
}