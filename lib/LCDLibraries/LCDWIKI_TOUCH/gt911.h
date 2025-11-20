#ifndef __GT911_DRIVER_H
#define __GT911_DRIVER_H

#include "cptiic.h"

#define TOUCH_MAX 5

#define GT9XX_IIC_RADDR 0xBB	//IIC read address, should be 0x29
#define GT9XX_IIC_WADDR 0xBA	//IIC write address, should be 0x28

#define GT9XX_READ_ADDR 0x814E	//touch point information
#define GT9XX_ID_ADDR 0x8140		//ID of touch IC

class GT911:public CPTIIC
{
  public:
  GT911(int8_t cptint, int8_t cptrst,int8_t cptscl, int8_t cptsda);	
	//IIC所有操作函数
  void GT911_int_sync(uint16_t ms);
  void GT911_reset_guitar(uint8_t addr);
  void GT911_gpio_init(int8_t cptint, int8_t cptrst,int8_t cptscl, int8_t cptsda);
  uint8_t GT9XX_WriteHandle (uint16_t addr);
  uint8_t GT9XX_WriteData (uint16_t addr,uint8_t value);
  uint8_t GT9XX_ReadData (uint16_t addr, uint8_t cnt, uint8_t *value);
  uint8_t GT911_Init(uint8_t r,uint16_t w, uint16_t h);
  uint8_t Touch_Get_Count(void);
  uint8_t GT911_Scan(void);
  void GT911_reset(void);

  int8_t gcptint;
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
