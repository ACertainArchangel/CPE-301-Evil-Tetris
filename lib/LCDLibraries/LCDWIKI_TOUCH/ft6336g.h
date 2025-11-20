#ifndef __FT6336G_DRIVER_H
#define __FT6336G_DRIVER_H
#include "cptiic.h"

#define TOUCH_MAX 2

//IIC command
#define FT6336_IIC_ADDR     0x38    //IIC address
#define FT6336_IIC_CMD_WR   0x70    //write command
#define FT6336_IIC_CMD_RD   0x71    //read command

//FT6336 regist define
#define FT6336_DEVIDE_MODE 			0x00   		//FT6336模式控制寄存器
#define FT6336_REG_NUM_FINGER       0x02		//触摸状态寄存器

#define FT6336_TP1_REG 				0X03	  	//第一个触摸点数据地址
#define FT6336_TP2_REG 				0X09		//第二个触摸点数据地址

#define	FT6336_ID_G_LIB_VERSION		0xA1		//版本		
#define FT6336_ID_G_MODE 			0xA4   		//FT6336中断模式控制寄存器
#define FT6336_ID_G_THGROUP			0x80   		//触摸有效值设置寄存器
#define FT6336_ID_G_PERIODACTIVE	0x88   		//激活状态周期设置寄存器
#define FT6336_ID_G_CIPHER_MID    0x9F
#define FT6336_ID_G_CIPHER_LOW    0xA0
#define FT6336_ID_G_CIPHER_HIGH   0xA3
#define FT6336_ID_G_FOCALTECH_ID  0xA8


class FT6336:public CPTIIC
{
  public:
	FT6336(int8_t cptint, int8_t cptrst,int8_t cptscl, int8_t cptsda);
	FT6336(int8_t cptint, int8_t cptrst);
	uint8_t FT6336_WR_Reg(uint16_t reg,uint8_t *buf,uint8_t len);
	void FT6336_RD_Reg(uint16_t reg,uint8_t *buf,uint8_t len);
	uint8_t FT6336_Init(uint8_t r,uint16_t w, uint16_t h);
	uint8_t FT6336_Scan(void); 
	
  	uint8_t ctp_status,lcd_r,hardware_iic;
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
