#include "ft5426.h"
#include "mcu_touch_magic.h"

#define TP_PRES_DOWN 0x80  
#define TP_CATH_PRES 0x40 

FT5426::FT5426(int8_t cptint, int8_t cptrst,int8_t cptscl, int8_t cptsda):CPTIIC(cptscl, cptsda)
{
	cptintPort = portInputRegister(digitalPinToPort(cptint));
	cptrstPort = portOutputRegister(digitalPinToPort(cptrst));
	
	cptintPinSet = digitalPinToBitMask(cptint);
	cptrstPinSet = digitalPinToBitMask(cptrst);
		
	pinMode(cptint, INPUT);	  
	pinMode(cptrst, OUTPUT);
	digitalWrite(cptint, HIGH);
	digitalWrite(cptrst, HIGH);
	ctp_status=0;
	x[5]={0},y[5]={0};
	lcd_h=0,lcd_w=0,lcd_r=0;
}

uint8_t FT5426::FT5426_WR_Reg(uint16_t reg,uint8_t *buf,uint8_t len)
{
	uint8_t i,ret=0;
	CT_IIC_Start();
	CT_IIC_Send_Byte(FT_IIC_CMD_WR);
	CT_IIC_Wait_Ack();
	CT_IIC_Send_Byte(reg&0xFF);
	CT_IIC_Wait_Ack();
	for(i=0;i<len;i++)
	{
		CT_IIC_Send_Byte(buf[i]);
		ret=CT_IIC_Wait_Ack();
		if(ret)
		{
			break;
		}
	}
	CT_IIC_Stop();
	return ret;
}

void FT5426::FT5426_RD_Reg(uint16_t reg,uint8_t *buf,uint8_t len)
{
	uint8_t i;
	CT_IIC_Start();
	CT_IIC_Send_Byte(FT_IIC_CMD_WR);
	CT_IIC_Wait_Ack();
	CT_IIC_Send_Byte(reg&0xFF);
	CT_IIC_Wait_Ack();
	CT_IIC_Start();
	CT_IIC_Send_Byte(FT_IIC_CMD_RD);
	CT_IIC_Wait_Ack();
	for(i=0;i<len;i++)
	{
		buf[i]=CT_IIC_Read_Byte(i==(len-1)?0:1);
	}
	CT_IIC_Stop();
}

uint8_t FT5426::FT5426_Init(uint8_t r,uint16_t w, uint16_t h)
{
	uint8_t temp[2];
	lcd_r=r;
	lcd_w=w;
	lcd_h=h;
	RST_CTRL_LOW;
	delay(20);
	RST_CTRL_HIGH;
	delay(50);
	temp[0]=0;
	FT5426_WR_Reg(FT_DEVIDE_MODE,temp,1);
	FT5426_WR_Reg(FT_ID_G_MODE,temp,1);
	temp[0]=22;
	FT5426_WR_Reg(FT_ID_G_THGROUP,temp,1);
	temp[0]=12;
	FT5426_WR_Reg(FT_ID_G_PERIODACTIVE,temp,1);
	FT5426_RD_Reg(FT_ID_G_LIB_VERSION,&temp[0],2);
	if(temp[0]==0X30&&temp[1]==0X03)
	{
		return 0;
	}
	return 1;
}

const uint16_t FT5426_TPX_TBL[5]={FT_TP1_REG,FT_TP2_REG,FT_TP3_REG,FT_TP4_REG,FT_TP5_REG};

uint8_t FT5426::FT5426_Scan(void)
{
	uint8_t buf[4],i,res,temp,mode;
 	static uint8_t t=0;//控制查询间隔,从而降低CPU占用率   
	t++;
	if((t%10)==0||t<10)//空闲时,每进入10次CTP_Scan函数才检测1次,从而节省CPU使用率
	{ 
		FT5426_RD_Reg(FT_REG_NUM_FINGER,&mode,1);
		if((mode&0XF)&&((mode&0XF)<6))
		{
			temp=0XFF<<(mode&0XF);		//将点的个数转换为1的位数,匹配tp_dev.sta定义 
			ctp_status=(~temp)|TP_PRES_DOWN|TP_CATH_PRES; 
			for(i=0;i<TOUCH_MAX;i++)
			{
				if(ctp_status&(1<<i))	//触摸有效?
				{
					FT5426_RD_Reg(FT5426_TPX_TBL[i],buf,4);	//读取XY坐标值
					if(lcd_r==2)
					{
						x[i]=((uint16_t)(buf[0]&0X0F)<<8)+buf[1];
						y[i]=lcd_h-(((uint16_t)(buf[2]&0X0F)<<8)+buf[3]);
					}
					else if(lcd_r==3)
					{
						y[i]=lcd_h-(((uint16_t)(buf[0]&0X0F)<<8)+buf[1]);
						x[i]=lcd_w-(((uint16_t)(buf[2]&0X0F)<<8)+buf[3]);
					}
					else if(lcd_r==0)
					{
						x[i]=lcd_w-(((uint16_t)(buf[0]&0X0F)<<8)+buf[1]);
						y[i]=((uint16_t)(buf[2]&0X0F)<<8)+buf[3];
						//x[i]=lcd_w-(uint32_t)(((uint16_t)(buf[0]&0X0F)<<8)+buf[1])*100/125;
						//y[i]=(uint32_t)(((uint16_t)(buf[2]&0X0F)<<8)+buf[3])*100/128;


					}
					else if(lcd_r==1)
					{
						y[i]=((uint16_t)(buf[0]&0X0F)<<8)+buf[1];
						x[i]=((uint16_t)(buf[2]&0X0F)<<8)+buf[3];
						//y[i]=(uint32_t)(((uint16_t)(buf[0]&0X0F)<<8)+buf[1])*100/125;
						//x[i]=(uint32_t)(((uint16_t)(buf[2]&0X0F)<<8)+buf[3])*100/128;						
					}
					if((buf[0]&0XF0)!=0X80)
					{
						x[i]=y[i]=0;
					}
				}			
			} 
			res=1;
			if(x[0]==0 && y[0]==0)
			{
				mode=0;
			}
			t=0;
		}
	}
	if((mode&0X1F)==0)//无触摸点按下
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



