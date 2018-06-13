#pragma once

#ifdef __cplusplus 
extern "C" {
	#endif  
	
	#include "stm32f10x.h"                // Device header
	
	typedef struct XY{
		int x;
		int y;
	}xy;
	typedef struct RT{
		int r;
		int t;
	}rt;
	
	//the property of one point
	struct LDRPoint{
		u8 zero;
		u8 quality;
		u16 angle;
		u16 distance;
	};
	//original datastructure
	static union LIDARDATA{
	  u8 data[6];
		LDRPoint Point;
	}LDRScan;
		


	#ifdef __cplusplus  
} 
#endif 

