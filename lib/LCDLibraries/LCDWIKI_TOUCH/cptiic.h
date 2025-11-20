#ifndef __MYCT_IIC_H
#define __MYCT_IIC_H

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class CPTIIC
{
  public:	
  	CPTIIC(int8_t cptscl, int8_t cptsda);
	//IIC所有操作函数
	void CT_IIC_Init(int8_t cptscl, int8_t cptsda); 		//初始化IIC的IO口				 
	void CT_IIC_Start(void);				//发送IIC开始信号
	void CT_IIC_Stop(void); 				//发送IIC停止信号
	void CT_IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
	uint8_t CT_IIC_Read_Byte(uint8_t ack); //IIC读取一个字节
	uint8_t CT_IIC_Wait_Ack(void);				//IIC等待ACK信号
	void CT_IIC_Ack(void);					//IIC发送ACK信号
	void CT_IIC_NAck(void); 				//IIC不发送ACK信号
	int8_t gcptsda,gcptscl;
		
  private:
// 	#ifdef __AVR__
		volatile uint8_t *cptsclPort, *cptsdaPort;
		uint8_t  cptsclPinSet, cptsdaPinSet;
//	#endif
};

#endif







