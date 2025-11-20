#include "ft6336g.h"
#include "mcu_touch_magic.h"
#include <Wire.h>

#define TP_PRES_DOWN 0x80  
#define TP_CATH_PRES 0x40 

FT6336::FT6336(int8_t cptint, int8_t cptrst,int8_t cptscl, int8_t cptsda):CPTIIC(cptscl, cptsda)
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
	hardware_iic=0;
	x[2]={0},y[2]={0};
	lcd_h=0,lcd_w=0,lcd_r=0;
}

FT6336::FT6336(int8_t cptint, int8_t cptrst):CPTIIC(-1,-1)
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
	hardware_iic=1;
	x[2]={0},y[2]={0};
	lcd_h=0,lcd_w=0,lcd_r=0;
	Wire.begin();
}

uint8_t FT6336::FT6336_WR_Reg(uint16_t reg,uint8_t *buf,uint8_t len)
{
	uint8_t ret=0;
	if(hardware_iic==1)
	{
		Wire.beginTransmission(FT6336_IIC_ADDR);
		Wire.write((byte)(reg&0xFF));
		ret = Wire.write(buf, len);
		Wire.endTransmission();
	}
	else
	{
		uint8_t i;
		CT_IIC_Start();
		CT_IIC_Send_Byte(FT6336_IIC_CMD_WR);
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
	}
	return ret;
}

void FT6336::FT6336_RD_Reg(uint16_t reg,uint8_t *buf,uint8_t len)
{
	uint8_t i;
	if(hardware_iic==1)
	{
		Wire.beginTransmission(FT6336_IIC_ADDR);
		Wire.write((byte)(reg&0xFF));
		Wire.endTransmission();
		Wire.requestFrom((byte)FT6336_IIC_ADDR, (byte)len);
		for(i=0;i<len;i++)
		{
			buf[i]=Wire.read();
		}
	}
	else
	{
		CT_IIC_Start();
		CT_IIC_Send_Byte(FT6336_IIC_CMD_WR);
		CT_IIC_Wait_Ack();
		CT_IIC_Send_Byte(reg&0xFF);
		CT_IIC_Wait_Ack();
		CT_IIC_Start();
		CT_IIC_Send_Byte(FT6336_IIC_CMD_RD);
		CT_IIC_Wait_Ack();
		for(i=0;i<len;i++)
		{
			buf[i]=CT_IIC_Read_Byte(i==(len-1)?0:1);
		}
		CT_IIC_Stop();
	}
}

uint8_t FT6336::FT6336_Init(uint8_t r,uint16_t w, uint16_t h)
{
	uint8_t temp[2];
	lcd_r=r;
	lcd_w=w;
	lcd_h=h;
	RST_CTRL_LOW;
	delay(20);
	RST_CTRL_HIGH;
	delay(500);
	//temp[0]=0;
	//FT6336_WR_Reg(FT6336_DEVIDE_MODE,temp,1);
	//FT6336_WR_Reg(FT6336_ID_G_MODE,temp,1);
	//temp[0]=22;
	//FT6336_WR_Reg(FT6336_ID_G_THGROUP,temp,1);
	//temp[0]=12;
	//FT6336_WR_Reg(FT6336_ID_G_PERIODACTIVE,temp,1);
	FT6336_RD_Reg(FT6336_ID_G_FOCALTECH_ID,&temp[0],1);
	if(temp[0]!=0x11)
	{
		return 1;
	}
	FT6336_RD_Reg(FT6336_ID_G_CIPHER_MID,&temp[0],2);
	if(temp[0]!=0x26)
	{
		return 1;
	}
	if((temp[1]!=0x00)&&(temp[1]!=0x01)&&(temp[1]!=0x02))
	{
		return 1;
	}
	FT6336_RD_Reg(FT6336_ID_G_CIPHER_HIGH,&temp[0],1);
	if(temp[0]!=0x64)
	{
		return 1;
	}
	return 0;
}

const uint16_t FT6336_TPX_TBL[2]={FT6336_TP1_REG,FT6336_TP2_REG};

uint8_t FT6336::FT6336_Scan(void)
{
	uint8_t buf[4],i,res,temp,mode;
 	static uint8_t t=0;//���Ʋ�ѯ���,�Ӷ�����CPUռ����   
	t++;
	if((t%10)==0||t<10)//����ʱ,ÿ����10��CTP_Scan�����ż��1��,�Ӷ���ʡCPUʹ����
	{ 
		FT6336_RD_Reg(FT6336_REG_NUM_FINGER,&mode,1);
		if(mode&&(mode<3))
		{
			temp=0XFF<<(mode&0XF);		//����ĸ���ת��Ϊ1��λ��,ƥ��tp_dev.sta���� 
			ctp_status=(~temp)|TP_PRES_DOWN|TP_CATH_PRES; 
			for(i=0;i<TOUCH_MAX;i++)
			{
				if(ctp_status&(1<<i))	//������Ч?
				{
					FT6336_RD_Reg(FT6336_TPX_TBL[i],buf,4);	//��ȡXY����ֵ
					if(lcd_r==2)
					{
						x[i]=lcd_w-(((uint16_t)(buf[0]&0X0F)<<8)+buf[1]);
						y[i]=lcd_h-(((uint16_t)(buf[2]&0X0F)<<8)+buf[3]);
					}
					else if(lcd_r==3)
					{
						y[i]=((uint16_t)(buf[0]&0X0F)<<8)+buf[1];
						x[i]=lcd_w-(((uint16_t)(buf[2]&0X0F)<<8)+buf[3]);
					}
					else if(lcd_r==0)
					{
						x[i]=((uint16_t)(buf[0]&0X0F)<<8)+buf[1];
						y[i]=((uint16_t)(buf[2]&0X0F)<<8)+buf[3];
						//x[i]=lcd_w-(uint32_t)(((uint16_t)(buf[0]&0X0F)<<8)+buf[1])*100/125;
						//y[i]=(uint32_t)(((uint16_t)(buf[2]&0X0F)<<8)+buf[3])*100/128;


					}
					else if(lcd_r==1)
					{
						y[i]=lcd_h-(((uint16_t)(buf[0]&0X0F)<<8)+buf[1]);
						x[i]=((uint16_t)(buf[2]&0X0F)<<8)+buf[3];
						//y[i]=(uint32_t)(((uint16_t)(buf[0]&0X0F)<<8)+buf[1])*100/125;
						//x[i]=(uint32_t)(((uint16_t)(buf[2]&0X0F)<<8)+buf[3])*100/128;						
					}
					//if((buf[0]&0XF0)!=0X80)
					//{
					//	x[i]=y[i]=0;
					//}
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
	if(mode==0)//�޴����㰴��
	{ 
		if(ctp_status&TP_PRES_DOWN)	//֮ǰ�Ǳ����µ�
		{
			ctp_status&=~(1<<7);	//��ǰ����ɿ�
		}else						//֮ǰ��û�б�����
		{ 
			x[0]=0xffff;
			y[0]=0xffff;
			ctp_status&=0XE0;	//�������Ч���	
		}	 
	} 	
	if(t>240)t=10;//���´�10��ʼ����
	return res;
}



