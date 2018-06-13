#include "usart.h"
#include "tim.h"
#include "delay.h"
#include "DataProcess.h"
#include "DataStructure.h"
#include <vector>

//relative Coordinate
u32 ReCoorr[1000];
//Absolute Coordinate
xy pxy[100];

//exist in usart.cpp and DataProcess.cpp
//the area of grid each edge is 400mm , field : 5000*17000

u8 grid[13][42];

//flag: ask Pose,and wait to the reply
int volatile AKFlag =0;

void gridinit()
{
	for(int i=0;i<13;i++) {
		for(int j=0;j<42;j++) {
			grid[i][j] = 0;
		}
	}
}

//the pos of the platform
union Posture{
	u8 volatile sepe[12];
	int  pos[3];
}Pose;

union Posture Pose0={(int)0,(int)0,(int)0};

//USARTx:USART1,2
//USART_IT_x:USART_IT_TXE/RXNE
//STATE: ENABLE DISABLE
void usart(USART_TypeDef *USARTx,uint16_t USART_IT_x,FunctionalState STATE)
{
	uint16_t GPIO_Pin_Tx;
	uint16_t GPIO_Pin_Rx;
	GPIO_TypeDef * GPIOx;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	if(USARTx==USART1) {
	  /* USART1 | GPIOA Clock Enable*/
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE); 
	  /* USART1 IO : TX-PA9   RX--PA10*/	
		GPIO_Pin_Tx = GPIO_Pin_9;
		GPIO_Pin_Rx = GPIO_Pin_10;
		GPIOx = GPIOA;
	}
	if(USARTx==USART2) {
		/* USART1 | GPIOA Clock Enable*/
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE); 
		/* USART2 IO : TX-PA2   RX--PA3*/	
		GPIO_Pin_Tx = GPIO_Pin_2;
		GPIO_Pin_Rx = GPIO_Pin_3;
		GPIOx = GPIOA;
	}
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_Tx;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;       //TX_AFPP
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOx, &GPIO_InitStructure);    
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_Rx;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//RX_INFLOATING
	GPIO_Init(GPIOx, &GPIO_InitStructure);
	
	/* USARTx Cinfig */
	USART_InitStructure.USART_BaudRate = 115200;	                //115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	  //8
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 	      //1
	USART_InitStructure.USART_Parity = USART_Parity_No ;          //N
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//None
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 //TX&&RX
	USART_Init(USARTx, &USART_InitStructure);
	USART_Cmd(USARTx, ENABLE);// USARTx ENABLE
	
	//interupt set
	USART_ITConfig(USARTx, USART_IT_x, STATE);
}

void USART_NVIC_Configuration()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;     //command interrupt is superior
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
 
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;   
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1; 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

//void USART_Send_nBytes(USART_TypeDef *USARTx,u16 data)
//{
//	for(int i=0;i<sizeof(data);i++) {
//		USART_SendData(USARTx,data>>(i*8));
//		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
//	}
//}

//void USART_Send_nBytes(USART_TypeDef *USARTx,u32 data)
//{
//	for(int i=0;i<sizeof(data);i++) {
//		USART_SendData(USARTx,data>>(i*8));
//		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
//	}
//}

void USART1_IRQHandler()
{
	char c;
	static int cnt=0;
	static u8 countst = 0,countop=0,countre=0;
	if(USART_GetITStatus(USART1, USART_IT_RXNE)==SET) {
		USART_ClearITPendingBit( USART1,USART_IT_RXNE);
		
    c=USART_ReceiveData(USART1);
		
		//Debug: print the data USART1 recieved
    //USART_SendData(USART1,c);
		//while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		
		//RE: Pose
		//AKFlag =0;
    switch(countre) {
			case 0:
				if(c=='R') {countre++;}
				else       countre=0;
				break;		 
      case 1:
				if(c=='E') { countre++; }
				else {if(c!='R') countre=0;}
				break; 
      case 2: 
				Pose.sepe[cnt++]=c;
			  if (cnt==12) {countre = 0;cnt=0;AKFlag=1;}
			  break;
			default:
				break;
		}

		//OP:stop
    switch(countop) {
			case 0:
				if(c=='O') countop++;
				else       countop=0;
				break;		 
      case 1:
				if(c=='P') {
					countop = 0;
					USART_SendData(USART2,0xA5); 
          while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
	        //DelayMs(5);
	        USART_SendData(USART2,0x25); 
          while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
					DelayMs(10);
				  TIM2_PWM_CH2(0);
					
					PathPlan_TwoArea();
					
					Pose0.pos[0]=0;
					Pose0.pos[1]=0;
				}
				else {if(c!='O') countop=0;}
				break; 
			default:break;
		}
		
		//ST: start start TIM2
    switch(countst) {
			case 0:
				if(c=='S') countst++;
				else       countst=0;
				break;		 
      case 1:
				if(c=='T') { 
				  countst = 0;
					//no goal
					gridinit();
					
					TIM2_PWM_CH2(59);
					//delay to wait la2 stable
					DelayMs(1500);
					USART_SendData(USART2,0xA5); 
          while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
	        USART_SendData(USART2,0x20); 
          while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
				}
				else {if(c!='S') countst=0;}
				break; 
			default: break;
		}	
	}
}


//USART2
void USART2_IRQHandler()
{

  //n:the size of ReCoor
	//count: check the data head
	//i: count the size of datas by Byte
	//whichturn: when == 3,the data is wanted
	//ch: what the data is
	
	static int n=0;
  static u8 count=0;
  static u8 i=0;
	static u8 whichturn=0;
	static volatile u8 ch;
	
	
	//u8 k=0;
	
  if(USART_GetITStatus(USART2, USART_IT_RXNE)==SET) {
		USART_ClearITPendingBit( USART2,USART_IT_RXNE);
    ch=USART_ReceiveData(USART2);
    
		//USART_SendData(USART1,ch);

    switch(count) {
			case 0:
				if(ch==0xa5)
					count++;
				else 
					count=0;
				break;		 
      case 1:
				if(ch==0x5a) {
					i=0;
					count++;
				}
				else {if(ch!=0xa5) count=0;}
				break; 
      case 2: 
				if(i==0) LDRScan.data[i]=0x00;
				//LDRScan.data[i+1]=ch;
			  //++i;
			  LDRScan.data[++i]=ch;
			  if(i==5) {
					//reload a new point
					i=0;
					if((LDRScan.Point.quality&0x01)==0x01)
						whichturn++;
					
					if(whichturn==3)
					{
						if((LDRScan.Point.angle)<20000) {
							if((LDRScan.Point.quality>>2)!=0) 
								ReCoorr[n++] = (u32)(LDRScan.Point.distance<<16)+LDRScan.Point.angle;
							if((LDRScan.Point.angle)>19800) goto A;  //when angle>125 , abandon the else
						}
						  
					}
					if(whichturn==4){
						
					A:
             ReCoorr[n]=0;
						
						//after one cirlce data collecting , flag reset;
						whichturn=0;  count=0; i=0;
						
						//disable usart2 rx interupt
						USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
						//stop ldr scan
						USART_SendData(USART2,0xA5); 
            while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
	          USART_SendData(USART2,0x25); 
            while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	
						
						//ask for Pose
						AKFlag=0;
            while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);						
						USART_SendData(USART1,'A'); 
            while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	          USART_SendData(USART1,'K'); 
            while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
						//wait to receive pos
						while(AKFlag==0);
						
						//记录路径规划开始位置坐标
						if(Pose0.pos[0]==0&&Pose0.pos[1]==0)
							Pose0 = Pose;
						
						
						//make the Pose msg is receiced
						//get ball recoor in regoal
            //DataProcess_1(ReCoor);
						//position transfer,get ball abcoor
						DataProcess_1(ReCoorr, pxy); 
						n=0;
							
		
						//reboot LA
						//collect msg very 0.7 + 0.5s
						DelayMs(300);
						
            USART_SendData(USART2,0xA5); 
            while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
	          USART_SendData(USART2,0x20); 
            while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);				
						//enable usart2 rx enable
						USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
					}
				}
				break;	  
			default:
				count=0;
			break;		 
		}
		
	}
}