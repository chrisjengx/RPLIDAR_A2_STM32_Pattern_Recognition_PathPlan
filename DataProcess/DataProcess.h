#pragma once

#ifdef __cplusplus 
extern "C" {
	#endif  
	
	
	#include "stm32f10x.h"                // Device header
	#include "DataStructure.h"
	//get ball recoor in regoal
	//position transfer,get ball abcoor
	
	#define JMAX 42
	
	void DataProcess_1(u32 *ReCoor, xy* pxy);
  
	//One as a whole
	//void PathPlan();
	
	//split into two areas
	void PathPlan_TwoArea();
	int pathplan(int path[],int imin, int imax, int jmin, int jmax);
	
	#ifdef __cplusplus  
} 
#endif 

