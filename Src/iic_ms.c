#include "iic_ms.h"

void delay_us(uint32_t nus)
{
   uint32_t temp;
   SysTick->LOAD = 8*nus;
   SysTick->VAL=0X00;//��ռ�����
   SysTick->CTRL=0X01;//ʹ�ܣ����������޶����������ⲿʱ��Դ
   do
   {
     temp=SysTick->CTRL;//��ȡ��ǰ������ֵ
   }while((temp&0x01)&&(!(temp&(1<<16))));//�ȴ�ʱ�䵽��
   SysTick->CTRL=0x00; //�رռ�����
   SysTick->VAL =0X00; //��ռ�����
}
void delay_ms(uint16_t nms)
{
   uint32_t temp;
   SysTick->LOAD = 8000*nms;
   SysTick->VAL=0X00;//��ռ�����
   SysTick->CTRL=0X01;//ʹ�ܣ����������޶����������ⲿʱ��Դ
   do
  {
    temp=SysTick->CTRL;//��ȡ��ǰ������ֵ
   }while((temp&0x01)&&(!(temp&(1<<16))));//�ȴ�ʱ�䵽��
   SysTick->CTRL=0x00; //�رռ�����
   SysTick->VAL =0X00; //��ռ�����
}

/*****************IIC��������******************/
void IIC_Start(void)				//����IIC��ʼ�ź�
{
	IIC_SCL_H;
	IIC_SDA_H;
	IIC_Delay;
	IIC_SDA_L;
	IIC_Delay;
	IIC_SCL_L;
}
	
void IIC_Stop(void)	  			//����IICֹͣ�ź�	
{
	IIC_SCL_L;
	IIC_SDA_L;
	IIC_Delay;
	IIC_SCL_H;
	IIC_Delay;
	IIC_SDA_H;	
}
	
void IIC_Ack(void)					//IIC����ACK�ź�
{
	IIC_SCL_L;
	IIC_SDA_L;
	IIC_Delay;
	IIC_SCL_H;
	IIC_Delay;
	IIC_SCL_L;
	IIC_SDA_H; //����ͷ�����
	
}

void IIC_NAck(void)				//IIC������ACK�ź�
{
	IIC_SCL_L;
	IIC_SDA_H;
	IIC_Delay;
	IIC_SCL_H;
	IIC_Delay;
	IIC_SCL_L;
}

uint8_t IIC_Wait_Ack(void)				//IIC�ȴ�ACK�ź�	
{
	int time = 0;
	IIC_SDA_H;
	IIC_Delay;
	IIC_SCL_H;
	IIC_Delay;
	while(READ_SDA)     //�ȴ�Ӧ���ź�
	{
		time++;
		if(time>Time_Count) //�ȴ�ʱ�����������ֹͣ�źţ�����1����ʾ����Ӧ��ʧ��
		{
			//IIC_Stop();
			return 1;
		}
	}	
	IIC_SCL_L;   //Ӧ��ɹ�����SCL���
	return 0;
}	

void IIC_Send_Byte(uint8_t data)	 //IIC����һ���ֽ�
{
	IIC_SCL_L;
	IIC_Delay;
	for(int i=0;i<8;i++)         //�Ӹ�λ��ʼһλһλ�ش���
	{		                        //�����ݷŵ���������
		if((data & 0x80)>0)      //��ǰ���λΪ1
		  IIC_SDA_H;            //����������
		else
			IIC_SDA_L;
		data<<=1;             //��������һλ
		IIC_Delay;
		IIC_SCL_H;
		IIC_Delay;
		IIC_SCL_L;          //��һ�����ݷ�����ϣ�Ϊ��һ�����ݷ���׼��
		IIC_Delay;
	}
}	

uint8_t IIC_Read_Byte(unsigned char ack)       //IIC��ȡһ���ֽ�	
{
	uint8_t read_Data = 0;
//	IIC_SDA_H;	                   //�ͷ����ߣ��˴����ͷţ����ͷ�
	for(int i=0;i<8;i++)
	{
		IIC_SCL_L;
		IIC_Delay;
		IIC_Delay;
		IIC_SCL_H;        //������ʼ�����ݣ��ӻ������ٸı������ˣ����ı�SDA�ĵ�ƽ
		read_Data<<=1;
		if(READ_SDA)
			read_Data++;
		IIC_Delay;
	}
  if(ack==0) //˵����������Ҫ���ӻ�Ӧ��
		IIC_NAck();
	else
		IIC_Ack();
	return read_Data;
}

uint8_t I2C_Master_Transmit( uint8_t DevAddress,uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	IIC_Start();
	IIC_Send_Byte(DevAddress<<1);
	if(!IIC_Wait_Ack())
  { 
		for(int i=0;i<Size;i++)
	   {
			  IIC_Send_Byte(pData[i]); 
			  if(IIC_Wait_Ack())
				{
					IIC_Stop();
				  return 1;
				}
		 }
	}
   IIC_Stop();
   return 0;		 
}

uint8_t I2C_Master_Receive( uint8_t DevAddress,uint8_t *pData, uint16_t Size, uint32_t Timeout)   //������ȡ
{
	IIC_Start();
	IIC_Send_Byte((DevAddress<<1)+1);
	if(!IIC_Wait_Ack())
  { 
		for(int i=0;i<Size;i++)
	   {
			  pData[i] = IIC_Read_Byte(1);
		 }
	}
   IIC_Stop();
   return 0;		 
}

/******************************�ӻ�����*****************************/
void IIC_Slave_Ack(void)
{
	int time = 0;			
	IIC_SDA_L;
	while(!READ_SCL)
	   {
			time++;
			if(time>Time_Count)
				return;
		}
	while(READ_SCL)
		{
			time++;
			if(time>Time_Count)
			return;
	  }
  IIC_Delay;	
	IIC_SDA_H;
			
}
void IIC_Slave_NAck(void)
{
	int time = 0;
	IIC_SDA_H;
	while(!READ_SCL)
		{
			time++;
			if(time>Time_Count)
				return;
		}
	while(READ_SCL)
		{
			time++;
			if(time>Time_Count)
			return;
	  }
	IIC_Delay;	
}

uint8_t IIC_Slave_Wait_Ack(void)				//IIC�ӻ��ȴ�ACK�ź�	
{
	int time = 0;
	IIC_SDA_H;
	//�ӻ���������һ��ʱ�ӵ�������֮ǰ�ȴ�ACK
	while(READ_SCL)
		{
			time++;
			if(time>Time_Count) //�ȴ�ʱ�����
			{
				return 1;
			}
		}
	while(!READ_SCL)
	{
		time++;
		if(time>Time_Count) //�ȴ�ʱ�����
		{
			return 1;
		}
	}
//ʱ��������֮���������ߵ�ƽ�ж��Ƿ�Ӧ��
	if(READ_SDA)     //��ʱʱ��Ӧ���ڸߵ�ƽ
		return 1;
	else           
		return 0;     //Ӧ��ɹ�
	
	
}	

void IIC_Slave_Send_Byte(uint8_t data)			//IIC�ӻ�����һ���ֽ�
{
	//��ʱ���봦�ڵ�һ��ʱ��������֮ǰ
	int time = 0;
	for(int i=0;i<8;i++)//�Ӹ�λ��ʼһλһλ�ش���
	{
    while(READ_SCL)  //�ȴ�ʱ�ӵ͵�ƽ
		{
			time++;
			if(time>Time_Count)
			return;
		} 
		if((data & 0x80)>0)
		 IIC_SDA_H;
		else
		 IIC_SDA_L;
		data<<=1; //��������һλ
		//��һ�����ݷ�����ϣ�Ϊ��һ�����ݷ���׼��
		while(!READ_SCL)
		{
			time++;
			if(time>Time_Count)
			return;
		}
	}
	
}

uint8_t IIC_Slave_Read_Byte(unsigned char ack)//IIC��ȡһ���ֽ�	
{
	uint8_t read_Data = 0;
	int time = 0;
	for(int i=0;i<8;i++)
	{
		//����׼��
		while(READ_SCL)
		{
			time++;
			if(time>Time_Count) //�ȴ�ʱ�����
			{
				return 1;
			}
		}
		while(!READ_SCL)
		{
			time++;
			if(time>Time_Count) //�ȴ�ʱ�����
			{
				return 1;
			}
		}
		read_Data<<=1; 
		if(READ_SDA)
			read_Data++;
		IIC_Delay;		
	}
  if(ack==0) //˵������ҪӦ��
		IIC_Slave_NAck();
	else
		IIC_Slave_Ack();
	return read_Data;
	
}

uint8_t wait_Start(void)
{
	int time = 0;
	if(READ_SCL && READ_SDA)
	{
		while(READ_SCL && READ_SDA)	
		{
			time++;
			if(time>Time_Count)
				return 1;
		}
		if(READ_SDA == 0 && READ_SCL == 1)
		 return 0;
    else 
	   return 1;
	}
	else
	 return 1;
}


uint8_t wait_Stop(void)
{
	int time = 0;
	while(!READ_SCL)
		{
			time++;
			if(time>Time_Count)
				return 1;
		}
	if(READ_SCL && !READ_SDA)
	{
		while(READ_SCL && !READ_SDA)	
		{
			time++;
			if(time>Time_Count)
				return 1;
		}
		if(READ_SDA == 1 && READ_SCL == 1)
		 return 0;
    else 
	   return 1;
	}
	else
	 return 1;
}

uint8_t I2C_Slave_Receive( uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	for(int i=Timeout;i!=0;i--)
	{
		if(!wait_Start())
		{
			if(IIC_Slave_Read_Byte(1) == IIC_Address<<1)
			{
				for(int i=0;i<Size;i++)
				{
					pData[i] = IIC_Slave_Read_Byte(1);
				}
			}
		else
			return 1;
		if(!wait_Stop())
		{
			return 0;
		}
		else
		  return 1;
			
		}
  }
}

uint8_t I2C_Slave_Transmit( uint8_t *pData, uint16_t Size, uint32_t Timeout) //IIC�ӻ���������
{
	for(int i=Timeout;i!=0;i--)
	{
		if(!wait_Start())
		{
			if(IIC_Slave_Read_Byte(1) == (IIC_Address<<1)+1)
			{
				for(int i=0;i<Size;i++)
				{
				  IIC_Slave_Send_Byte(pData[i]);
					IIC_SDA_H;
					if(IIC_Slave_Wait_Ack())  //IIC�ӻ��ȴ�ACK�ź�	
            return 1;              						
				}
			}
		else
			return 1;
		if(!wait_Stop())
		{
			return 0;
		}
		else
		  return 1;
			
		}
  }
}

