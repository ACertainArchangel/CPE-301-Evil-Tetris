#include "gt911.h"
#include "mcu_touch_magic.h"

#define TP_PRES_DOWN 0x80  
#define TP_CATH_PRES 0x40 


GT911::GT911(int8_t cptint, int8_t cptrst,int8_t cptscl, int8_t cptsda):CPTIIC(cptscl, cptsda)
{
#ifdef __AVR__
	cptintPort = portOutputRegister(digitalPinToPort(cptint));
	cptrstPort = portOutputRegister(digitalPinToPort(cptrst));
	
	cptintPinSet = digitalPinToBitMask(cptint);
	cptrstPinSet = digitalPinToBitMask(cptrst);
		
	pinMode(cptint, OUTPUT);	  
	pinMode(cptrst, OUTPUT);
#endif
	gcptint=cptint;
	ctp_status=0;
	x[5]={0},y[5]={0};
	lcd_h=0,lcd_w=0,lcd_r=0;	
}

//==================================================================
//Name:						GT911_int_sync
//Author:					QD
//Date:						2017-07-06
//Function:				Sync GT911 INT signal
//								Sync is the most important part in initial stage
//Input:					unsigned int
//Output:					void
//option:
//==================================================================
void GT911::GT911_int_sync(uint16_t ms)
{
	INT_CTRL_LOW;
	delay(ms);
	pinMode(gcptint, INPUT);
}	

//==================================================================
//Name:						GT911_reset_guitar
//Author:					QD
//Date:						2017-07-06
//Function:				Rest GT911, and work with function GT911_int_sync
//								sync sequence depends on slave address
//Input:					unsigned char addr
//Output:					void
//option:
//==================================================================
void GT911::GT911_reset_guitar(uint8_t addr)
{
	INT_CTRL_HIGH;
	RST_CTRL_HIGH;
	delay(20);
	INT_CTRL_LOW;
	RST_CTRL_LOW;
	delay(20);
	if(addr == 0x28)
		INT_CTRL_HIGH;
	else
		INT_CTRL_LOW;
	delay(20);
	RST_CTRL_HIGH;
	delay(20);
}

//==================================================================
//Name:						GT911_reset
//Author:					QD
//Date:						2017-07-06
//Function:				Rest GT911, just reset without sync
//Input:					void
//Output:					void
//option:
//==================================================================
void GT911::GT911_reset(void)
{
	RST_CTRL_LOW;
	delay(10);
	RST_CTRL_HIGH;
	delay(10);
}

//==================================================================
//Name:						GT9XX_WriteHandle
//Author:					QD
//Date:						2017-07-06
//Function:				Handle GT911 write function
//Input:					unsigned int addr
//Output:					void
//option:
//==================================================================
uint8_t GT911::GT9XX_WriteHandle (uint16_t addr)
{
	uint8_t status=0;
	CT_IIC_Start();
	CT_IIC_Send_Byte(GT9XX_IIC_WADDR); //写数据指令
	CT_IIC_Wait_Ack();
	CT_IIC_Send_Byte((uint8_t)(addr >> 8)); //写入16位地址
	CT_IIC_Wait_Ack();
	CT_IIC_Send_Byte((uint8_t)addr);
	CT_IIC_Wait_Ack();
	status = 1;
	return status;	
}

//==================================================================
//Name:						GT9XX_WriteData
//Author:					QD
//Date:						2017-07-06
//Function:				Write data to GT911
//Input:					unsigned int addr				//address of register
//								unsigned char value			//value should be writed
//Output:					void
//option:
//==================================================================
uint8_t GT911::GT9XX_WriteData (uint16_t addr,uint8_t value)
{
	uint8_t status=0;
	CT_IIC_Start();

	GT9XX_WriteHandle(addr);
	CT_IIC_Send_Byte(value);
	CT_IIC_Wait_Ack();
	CT_IIC_Stop();	
	status = 1;
	return status;
}

//==================================================================
//Name:						GT9XX_ReadData
//Author:					QD
//Date:						2017-07-06
//Function:				Read data from GT911
//Input:					unsigned int addr				//address of register
//								unsigned char *value		//pointer of data output
//Output:					void
//option:
//==================================================================
uint8_t GT911::GT9XX_ReadData (uint16_t addr, uint8_t cnt, uint8_t *value)
{
	uint8_t status;
	uint8_t i;

	status = 0;
	CT_IIC_Start();
	GT9XX_WriteHandle(addr);
	CT_IIC_Start();
	delayMicroseconds(20);
	CT_IIC_Send_Byte(GT9XX_IIC_RADDR);
	CT_IIC_Wait_Ack();
	for(i = 0 ; i < cnt; i++)
	{
		if (i == (cnt - 1))
		{
			value[i] = CT_IIC_Read_Byte(0);
		}
		else
		{
			value[i] = CT_IIC_Read_Byte(1);
		}
	}					
	CT_IIC_Stop();
	status = 1;
	return (status);	
}


//==================================================================
//Name:						Touch_Init
//Author:					QD
//Date:						2017-07-06
//Function:				GT911 init, including GPIO init, sync, and version check
//Input:					unsigned int addr				//address of register
//								unsigned char *value		//pointer of data output
//Output:					void
//option:
//==================================================================
uint8_t GT911::GT911_Init(uint8_t r,uint16_t w, uint16_t h)
{
	uint8_t touchIC_ID[4];
	GT911_reset();
	GT911_reset_guitar(GT9XX_IIC_WADDR);
	GT911_int_sync(50);
	GT9XX_ReadData (GT9XX_ID_ADDR,4,touchIC_ID);
	lcd_r=r;
	lcd_w=w;
	lcd_h=h;
	if( touchIC_ID[0] == '9' )
	{
		//printf("Touch ID: %s \r\n",touchIC_ID);
		//GT9xx_send_config();
		return 1;
	}
	else
	{
		//printf("Touch Error\r\n");
		return 0;
	}
}

uint8_t GT911::Touch_Get_Count(void)
{
	uint8_t count[1] = {0};
	GT9XX_ReadData (GT9XX_READ_ADDR,1,count);	//read touch data
	return (count[0]&0x0f);
}

const uint16_t TPX[] = {0x8150,0x8158,0x8160,0x8168,0x8170}; //电容屏触摸点数据地址（1~5）

//==================================================================
//Name:						Touch_Get_Data
//Author:					QD
//Date:						2017-07-06
//Function:				Get GT911 data, such as point and coordinate
//Input:					void
//Output:					void
//option:
//==================================================================
uint8_t GT911::GT911_Scan(void)
{
	uint8_t buf[4];
	uint8_t i=0;
	uint8_t res=0;
	uint8_t temp,mode;
	uint8_t tempsta;
 	static uint8_t t=0;//控制查询间隔,从而降低CPU占用率   
	t++;
	if((t%10)==0||t<10)//空闲时,每进入10次CTP_Scan函数才检测1次,从而节省CPU使用率
	{ 
 		GT9XX_ReadData(GT9XX_READ_ADDR, 1, &mode);
		if(mode&0X80&&((mode&0XF)<6))
		{
			temp=0;	
			GT9XX_WriteData (GT9XX_READ_ADDR,temp);
		}
		if((mode&0XF)&&((mode&0XF)<6))
		{
			temp=0XFF<<(mode&0XF);		//将点的个数转换为1的位数,匹配tp_dev.sta定义 
			tempsta=ctp_status;			//保存当前的tp_dev.sta值
			ctp_status=(~temp)|TP_PRES_DOWN|TP_CATH_PRES; 
			x[4]=x[0];	//保存触点0的数据
			y[4]=y[0];
			for(i=0;i<5;i++)
			{
				if(ctp_status&(1<<i))	//触摸有效?
				{
					GT9XX_ReadData(TPX[i],4,buf);	//读取XY坐标值
					if(lcd_r==2)
					{
						x[i]=((uint16_t)buf[1]<<8)+buf[0];
						y[i]=((uint16_t)buf[3]<<8)+buf[2];
					}
					else if(lcd_r==3)
					{
						y[i]=lcd_h-(((uint16_t)buf[1]<<8)+buf[0]);
						x[i]=((uint16_t)buf[3]<<8)+buf[2];
					}
					else if(lcd_r==0)
					{
						x[i]=lcd_w-(((uint16_t)buf[1]<<8)+buf[0]);
						y[i]=lcd_h-(((uint16_t)buf[3]<<8)+buf[2]);
					}
					else if(lcd_r==1)
					{
						y[i]=((uint16_t)buf[1]<<8)+buf[0];
						x[i]=lcd_w-(((uint16_t)buf[3]<<8)+buf[2]);						
					}
				}			
			} 
			res=1;
			if(x[0]>lcd_w||y[0]>lcd_h)//非法数据(坐标超出了)
			{ 
				if((mode&0XF)>1)		//有其他点有数据,则复第二个触点的数据到第一个触点.
				{
					x[0]=x[1];
					y[0]=y[1];
					t=0;				//触发一次,则会最少连续监测10次,从而提高命中率
				}else					//非法数据,则忽略此次数据(还原原来的)  
				{
					x[0]=x[4];
					y[0]=y[4];
					mode=0X80;		
					ctp_status=tempsta;	//恢复tp_dev.sta
				}
			}else t=0;					//触发一次,则会最少连续监测10次,从而提高命中率
		}
	}
	if((mode&0X8F)==0X80)//无触摸点按下
	{ 
		if(ctp_status&TP_PRES_DOWN)	//之前是被按下的
		{
			ctp_status&=~(1<<7);	//标记按键松开
		}else						//之前就没有被按下
		{ 
			x[0]=0xffff;
			y[0]=0xffff;
			ctp_status&=0XE0;	//清除点有效标记	
		}	 
	} 	
	if(t>240)t=10;//重新从10开始计数
	return res;
}


