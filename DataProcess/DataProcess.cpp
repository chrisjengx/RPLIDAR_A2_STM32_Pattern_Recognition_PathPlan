#include "DataProcess.h"
#include "math.h"
#include <vector>

//global variable in usart.cpp
//extern union Posrture Pose;
extern union Posture{
	u8 sepe[12];
	int  pos[3];
}Pose;

extern union Posture Pose0;
extern u8 grid[13][42];

//get ball recoor in regoal
void DataProcess_1(u32 *ReCoor, xy* pxy)
{
	//the absolute coordinary
	//int px, py;
	
	double distance=0, angle=0;
	
	//quantity of balls
	u8 cnt=0;
	
	bool ff=false;                       //wait to find the first which is likey to be a ball
	double lastdistance=0,lastangle=0;    //record the last piont message
	double ang1=0,ang2=0;                 //angle1 < angle2
	u8 n=0;                              //count the laser point
	//*********************************//
	
	for(int i=0;ReCoor[i]!=0;i++) {
    //high 16b :distance 
		//low :angle
		distance= (ReCoor[i]>>16 & (u32)0x0000FFFF)>>2;
		angle   = (ReCoor[i] & (u32)0x0000FFFF)/128.0;
		
		
		//abandon the data out of this area, and set their distance with 4000mm
		
		//if(angle<2 || angle>358 || (angle>120 && angle<270)) distance=3000;
		if(angle<4) distance=5000;
		if(angle>150) distance=5000;
    if(angle>153) {return;}
		if(ff==true) {
			
			if((lastdistance/distance> 0.95 && lastdistance/distance <1.05)) ++n;
			

			
			//find one tennis ball
			else {
				ang2=lastangle;
				//confirm it is or not    25cm(1400)--65cm(3725)  scan area r=2.3m	
				//test rmax = 1.8m
				if( n>1 && lastdistance<1200 && (lastdistance*(ang2-ang1))>1000 && (lastdistance*(ang2-ang1))<4500 ) {
	        //if it is, save the relative position
					//Pose.pos[0] -= Pose0.pos[0];
					//Pose.pos[1] -= Pose0.pos[1];
					
          pxy[cnt].x = (int)(lastdistance * sin((lastangle-Pose.pos[2])/57.2958)+ Pose.pos[0] +250*cos(0.7+Pose.pos[2]/57.2958));  //0.5515与雷达固定有关 与x右方向夹角
			    pxy[cnt].y = (int)(lastdistance * cos((lastangle-Pose.pos[2])/57.2958)+ Pose.pos[1] +250*sin(0.7+Pose.pos[2]/57.2958));	//298为雷达到系统中心距离

					
					//wang ge fa  : 400mm
					if(pxy[cnt].x % 400 < 200)  {pxy[cnt].x=pxy[cnt].x/400;	 }
					else                        {pxy[cnt].x=pxy[cnt].x/400+1;}
					if(pxy[cnt].y % 400 < 200)  {pxy[cnt].y=pxy[cnt].y/400; }
					else                        {pxy[cnt].y=pxy[cnt].y/400+1;}
					
					//debug:		
					//USART_SendData(USART1,pxy[cnt].x);
					//while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
					//USART_SendData(USART1,pxy[cnt].y);
					//while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);

					//tennis ball position in grid 
					//Y<8M
					if(pxy[cnt].x>=0 && pxy[cnt].x<13 && pxy[cnt].y<20 && pxy[cnt].y >=0)
					  grid[pxy[cnt].x][pxy[cnt].y]++;
					
          cnt++;
				}
				
				//after one ball, falgs reset
				ff=false;ang1=0;ang2=0;n=0;
			}
		}
			//judge if it can possibly be a ball;
			if(lastdistance / distance>1.15 ||lastdistance / distance < 0.85)  {ff=true;ang1=angle;n=0;}
			//record the last laser piont
			lastdistance=distance; lastangle=angle;
	}
}



//out:x*100+y
void PathPlan_TwoArea()
{
	//first and last position in y axis
	int p,p1,n;
	int path[90],path1[90];
	//p1=pathplan(path1,0,6,0,42);
	//p=pathplan(path,6,13,0,42);
	
	p1=pathplan(path1,0,6,0,18);  //0-6，6-13
	//p=pathplan(path,6,13,0,18);
	
	n=p+p1;
	path[n]=0xAB;
	//data head
	USART_SendData(USART1,'P');
  while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	USART_SendData(USART1,'L');
  while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	
	
	USART_SendData(USART1,n);
  while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	USART_SendData(USART1,n>>8);
  while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	USART_SendData(USART1,n>>16);
  while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	USART_SendData(USART1,n>>24);
  while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	
	for(int i=0;i<p;i++)
	{
		USART_SendData(USART1,path[i]);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	  USART_SendData(USART1,path[i]>>8);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		USART_SendData(USART1,path[i]>>16);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	  USART_SendData(USART1,path[i]>>24);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	}
		for(int i=p1-1;i>=0;i--)
	{
		USART_SendData(USART1,path1[i]);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	  USART_SendData(USART1,path1[i]>>8);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		USART_SendData(USART1,path1[i]>>16);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	  USART_SendData(USART1,path1[i]>>24);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	}
		USART_SendData(USART1,0xAB);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	
}


//vector<int> 
int pathplan(int path[],int imin, int imax, int jmin, int jmax)
{
	
	int lfirst = 0, llast = 0;
	int n=0;
	for (int j = 0;j < jmax;j++) {
		lfirst = 0; llast = 0;
		for (int i = imin, ii = imax - 1;i<imax && ii >= 0;i++, ii--) {
			if (grid[i][j] > 1 && lfirst == 0) {
				lfirst = i * 100 + j;
			}
			if (grid[ii][j] >1 && llast == 0) {
				llast = ii * 100 + j;
			}
			if (lfirst&&llast)
				break;
		}
		if (llast == 0 && lfirst == 0)
			continue;
		if (j % 2) {
			if (lfirst != 0)
				path[n++]=lfirst;
			if (llast != 0 && llast != lfirst)
				path[n++]=llast;
		}
		else {
			if (llast != 0)
				path[n++]=llast;
			if (lfirst != 0 && llast != lfirst)
				path[n++]=lfirst;
		}
	}
	return n;
}
