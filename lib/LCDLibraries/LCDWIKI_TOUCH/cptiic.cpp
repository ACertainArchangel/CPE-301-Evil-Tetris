#include "cptiic.h"
#include "mcu_touch_magic.h"

//����I2C�ٶȵ���ʱ
void CT_Delay(uint32_t us)
{
	uint32_t time=100*us/7;    
    while(--time); 
} 

//���ݴ���оƬIIC�ӿڳ�ʼ��
CPTIIC::CPTIIC(int8_t cptscl, int8_t cptsda)
{					     
//#ifdef __AVR__
//	cptsclPort = portOutputRegister(digitalPinToPort(cptscl));
//	cptsdaPort = portOutputRegister(digitalPinToPort(cptsda));

//	cptsclPinSet = digitalPinToBitMask(cptscl);
//	cptsdaPinSet = digitalPinToBitMask(cptsda);
	
//	pinMode(cptscl, OUTPUT);	  // Enable outputs
//	pinMode(cptsda, OUTPUT);
//#endif
	digitalWrite(cptscl, HIGH);
	digitalWrite(cptsda, HIGH);
	pinMode(cptscl, OUTPUT);
	pinMode(cptsda, OUTPUT);
	gcptsda=cptsda;
	gcptscl=cptscl;
}

//����IIC��ʼ�ź�
void CPTIIC::CT_IIC_Start(void)
{
	pinMode(gcptsda, OUTPUT);     //sda�����
	//CT_IIC_SDA_HIGH;
	digitalWrite(gcptsda, HIGH);
	//CT_IIC_SCL_HIGH;
	digitalWrite(gcptscl, HIGH);
	delayMicroseconds(1);
 	//CT_IIC_SDA_LOW;//START:when CLK is high,DATA change form high to low 
	digitalWrite(gcptsda, LOW);
	delayMicroseconds(1);
	//CT_IIC_SCL_LOW;//ǯסI2C���ߣ�׼�����ͻ�������� 
	digitalWrite(gcptscl, LOW);
}

//����IICֹͣ�ź�
void CPTIIC::CT_IIC_Stop(void)
{ 
	pinMode(gcptsda, OUTPUT);     //sda�����
	//CT_IIC_SCL_LOW;
	digitalWrite(gcptscl, LOW);
	//CT_IIC_SDA_LOW;
	digitalWrite(gcptsda, LOW);
	delayMicroseconds(1);
	//CT_IIC_SCL_HIGH;
	digitalWrite(gcptscl, HIGH);
	delayMicroseconds(1);
	//CT_IIC_SDA_HIGH;//STOP:when CLK is high DATA change form low to high 
	digitalWrite(gcptsda, HIGH);
}

//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
uint8_t CPTIIC::CT_IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	pinMode(gcptsda, INPUT);      //SDA����Ϊ����  
	//CT_IIC_SDA_HIGH;
	digitalWrite(gcptsda, HIGH);
	delayMicroseconds(1);	   
	//CT_IIC_SCL_HIGH;
	digitalWrite(gcptscl, HIGH);
	delayMicroseconds(1);	 
	//while(CT_IIC_SDA_STATE)
	while(digitalRead(gcptsda))
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			CT_IIC_Stop();
			return 1;
		} 
	}
	//CT_IIC_SCL_LOW;//ʱ�����0 	   
	digitalWrite(gcptscl, LOW);
	return 0;  
} 

//����ACKӦ��
void CPTIIC::CT_IIC_Ack(void)
{
	//CT_IIC_SCL_LOW;
	digitalWrite(gcptscl, LOW);
	pinMode(gcptsda, OUTPUT);     //sda�����
	//CT_IIC_SDA_LOW;
	digitalWrite(gcptsda, LOW);
	delayMicroseconds(1);
	//CT_IIC_SCL_HIGH;
	digitalWrite(gcptscl, HIGH);
	delayMicroseconds(1);
	//CT_IIC_SCL_LOW;
	digitalWrite(gcptscl, LOW);
}
//������ACKӦ��		    
void CPTIIC::CT_IIC_NAck(void)
{
	//CT_IIC_SCL_LOW;
	digitalWrite(gcptscl, LOW);
	pinMode(gcptsda, OUTPUT);     //sda�����
	//CT_IIC_SDA_HIGH;
	digitalWrite(gcptsda, HIGH);
	delayMicroseconds(1);
	//CT_IIC_SCL_HIGH;
	digitalWrite(gcptscl, HIGH);
	delayMicroseconds(1);
	//CT_IIC_SCL_HIGH;
	digitalWrite(gcptscl, HIGH);
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void CPTIIC::CT_IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	pinMode(gcptsda, OUTPUT);     //sda����� 	    
    //CT_IIC_SCL_LOW;//����ʱ�ӿ�ʼ���ݴ���
	digitalWrite(gcptscl,LOW);
	for(t=0;t<8;t++)
    {              
        if((txd&0x80)>>7)
        {
			//CT_IIC_SDA_HIGH;
			digitalWrite(gcptsda,HIGH);
		}
		else
		{
			//CT_IIC_SCL_LOW;
			digitalWrite(gcptsda,LOW);
		}
        txd<<=1; 	      
		//CT_IIC_SCL_HIGH;
		digitalWrite(gcptscl,HIGH);
		delayMicroseconds(1);
		//CT_IIC_SCL_LOW;	
		digitalWrite(gcptscl,LOW);
		delayMicroseconds(1);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
uint8_t CPTIIC::CT_IIC_Read_Byte(uint8_t ack)
{
	uint8_t i,receive=0;
 	pinMode(gcptsda, INPUT);      //SDA����Ϊ���� 
    for(i=0;i<8;i++ )
	{
        //CT_IIC_SCL_LOW;
        digitalWrite(gcptscl,LOW);
		delayMicroseconds(3);
		//CT_IIC_SCL_HIGH;  
		digitalWrite(gcptscl,HIGH);
		receive<<=1;
		if(digitalRead(gcptsda))
		{
			receive += 1;
		}
		else
		{
			receive += 0;
		}
	}	  				 
	if (!ack)CT_IIC_NAck();//����nACK
	else CT_IIC_Ack(); //����ACK   
 	return receive;
}




























