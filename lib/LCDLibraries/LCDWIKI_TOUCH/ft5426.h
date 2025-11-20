#ifndef __FT5426_DRIVER_H
#define __FT5426_DRIVER_H
#include "cptiic.h"

#define TOUCH_MAX 5

//IIC command
#define FT_IIC_CMD_WR   0x70    //write command
#define FT_IIC_CMD_RD   0x71    //read command

//FT5426 regist define
#define FT_DEVIDE_MODE 			0x00   		//FT5206模式控制寄存器
#define FT_REG_NUM_FINGER       0x02		//触摸状态寄存器

#define FT_TP1_REG 				0X03	  	//第一个触摸点数据地址
#define FT_TP2_REG 				0X09		//第二个触摸点数据地址
#define FT_TP3_REG 				0X0F		//第三个触摸点数据地址
#define FT_TP4_REG 				0X15		//第四个触摸点数据地址
#define FT_TP5_REG 				0X1B		//第五个触摸点数据地址  
 

#define	FT_ID_G_LIB_VERSION		0xA1		//版本		
#define FT_ID_G_MODE 			0xA4   		//FT5426中断模式控制寄存器
#define FT_ID_G_THGROUP			0x80   		//触摸有效值设置寄存器
#define FT_ID_G_PERIODACTIVE	0x88   		//激活状态周期设置寄存器

class FT5426:public CPTIIC
{
  public:
	FT5426(int8_t cptint, int8_t cptrst,int8_t cptscl, int8_t cptsda);	
	uint8_t FT5426_WR_Reg(uint16_t reg,uint8_t *buf,uint8_t len);
	void FT5426_RD_Reg(uint16_t reg,uint8_t *buf,uint8_t len);
	uint8_t FT5426_Init(uint8_t r,uint16_t w, uint16_t h);
	uint8_t FT5426_Scan(void); 
	
  	uint8_t ctp_status,lcd_r;
  	uint16_t x[TOUCH_MAX],y[TOUCH_MAX],lcd_w,lcd_h;
  	
  private:
 #ifdef __AVR__
	volatile uint8_t *cptintPort, *cptrstPort;
	uint8_t  cptintPinSet, cptrstPinSet;
 #else
	volatile uint32_t *cptintPort, *cptrstPort;
	uint32_t  cptintPinSet, cptrstPinSet;
#endif
};


#endif
