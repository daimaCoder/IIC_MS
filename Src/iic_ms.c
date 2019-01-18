#include "iic_ms.h"

void delay_us(uint32_t nus)
{
   uint32_t temp;
   SysTick->LOAD = 8*nus;
   SysTick->VAL=0X00;//清空计数器
   SysTick->CTRL=0X01;//使能，减到零是无动作，采用外部时钟源
   do
   {
     temp=SysTick->CTRL;//读取当前倒计数值
   }while((temp&0x01)&&(!(temp&(1<<16))));//等待时间到达
   SysTick->CTRL=0x00; //关闭计数器
   SysTick->VAL =0X00; //清空计数器
}
void delay_ms(uint16_t nms)
{
   uint32_t temp;
   SysTick->LOAD = 8000*nms;
   SysTick->VAL=0X00;//清空计数器
   SysTick->CTRL=0X01;//使能，减到零是无动作，采用外部时钟源
   do
  {
    temp=SysTick->CTRL;//读取当前倒计数值
   }while((temp&0x01)&&(!(temp&(1<<16))));//等待时间到达
   SysTick->CTRL=0x00; //关闭计数器
   SysTick->VAL =0X00; //清空计数器
}

/*****************IIC主机部分******************/
void IIC_Start(void)				//发送IIC开始信号
{
	IIC_SCL_H;
	IIC_SDA_H;
	IIC_Delay;
	IIC_SDA_L;
	IIC_Delay;
	IIC_SCL_L;
}
	
void IIC_Stop(void)	  			//发送IIC停止信号	
{
	IIC_SCL_L;
	IIC_SDA_L;
	IIC_Delay;
	IIC_SCL_H;
	IIC_Delay;
	IIC_SDA_H;	
}
	
void IIC_Ack(void)					//IIC发送ACK信号
{
	IIC_SCL_L;
	IIC_SDA_L;
	IIC_Delay;
	IIC_SCL_H;
	IIC_Delay;
	IIC_SCL_L;
	IIC_SDA_H; //添加释放总线
	
}

void IIC_NAck(void)				//IIC不产生ACK信号
{
	IIC_SCL_L;
	IIC_SDA_H;
	IIC_Delay;
	IIC_SCL_H;
	IIC_Delay;
	IIC_SCL_L;
}

uint8_t IIC_Wait_Ack(void)				//IIC等待ACK信号	
{
	int time = 0;
	IIC_SDA_H;
	IIC_Delay;
	IIC_SCL_H;
	IIC_Delay;
	while(READ_SDA)     //等待应答信号
	{
		time++;
		if(time>Time_Count) //等待时间过长，产生停止信号，返回1，表示接收应答失败
		{
			//IIC_Stop();
			return 1;
		}
	}	
	IIC_SCL_L;   //应答成功，则SCL变低
	return 0;
}	

void IIC_Send_Byte(uint8_t data)	 //IIC发送一个字节
{
	IIC_SCL_L;
	IIC_Delay;
	for(int i=0;i<8;i++)         //从高位开始一位一位地传送
	{		                        //发数据放到数据线上
		if((data & 0x80)>0)      //当前最高位为1
		  IIC_SDA_H;            //拉高数据线
		else
			IIC_SDA_L;
		data<<=1;             //数据左移一位
		IIC_Delay;
		IIC_SCL_H;
		IIC_Delay;
		IIC_SCL_L;          //上一个数据发送完毕，为下一个数据发送准备
		IIC_Delay;
	}
}	

uint8_t IIC_Read_Byte(unsigned char ack)       //IIC读取一个字节	
{
	uint8_t read_Data = 0;
//	IIC_SDA_H;	                   //释放总线，此处不释放，另处释放
	for(int i=0;i<8;i++)
	{
		IIC_SCL_L;
		IIC_Delay;
		IIC_Delay;
		IIC_SCL_H;        //主机开始读数据，从机不能再改变数据了，即改变SDA的电平
		read_Data<<=1;
		if(READ_SDA)
			read_Data++;
		IIC_Delay;
	}
  if(ack==0) //说明主机不需要给从机应答
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

uint8_t I2C_Master_Receive( uint8_t DevAddress,uint8_t *pData, uint16_t Size, uint32_t Timeout)   //主机读取
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

/******************************从机部分*****************************/
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

uint8_t IIC_Slave_Wait_Ack(void)				//IIC从机等待ACK信号	
{
	int time = 0;
	IIC_SDA_H;
	//从机必须在下一次时钟的上升沿之前等待ACK
	while(READ_SCL)
		{
			time++;
			if(time>Time_Count) //等待时间过长
			{
				return 1;
			}
		}
	while(!READ_SCL)
	{
		time++;
		if(time>Time_Count) //等待时间过长
		{
			return 1;
		}
	}
//时钟上升沿之后检测数据线电平判断是否应答
	if(READ_SDA)     //此时时钟应处于高电平
		return 1;
	else           
		return 0;     //应答成功
	
	
}	

void IIC_Slave_Send_Byte(uint8_t data)			//IIC从机发送一个字节
{
	//此时必须处于第一个时钟上升沿之前
	int time = 0;
	for(int i=0;i<8;i++)//从高位开始一位一位地传送
	{
    while(READ_SCL)  //等待时钟低电平
		{
			time++;
			if(time>Time_Count)
			return;
		} 
		if((data & 0x80)>0)
		 IIC_SDA_H;
		else
		 IIC_SDA_L;
		data<<=1; //数据左移一位
		//上一个数据发送完毕，为下一个数据发送准备
		while(!READ_SCL)
		{
			time++;
			if(time>Time_Count)
			return;
		}
	}
	
}

uint8_t IIC_Slave_Read_Byte(unsigned char ack)//IIC读取一个字节	
{
	uint8_t read_Data = 0;
	int time = 0;
	for(int i=0;i<8;i++)
	{
		//数据准备
		while(READ_SCL)
		{
			time++;
			if(time>Time_Count) //等待时间过长
			{
				return 1;
			}
		}
		while(!READ_SCL)
		{
			time++;
			if(time>Time_Count) //等待时间过长
			{
				return 1;
			}
		}
		read_Data<<=1; 
		if(READ_SDA)
			read_Data++;
		IIC_Delay;		
	}
  if(ack==0) //说明不需要应答
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

uint8_t I2C_Slave_Transmit( uint8_t *pData, uint16_t Size, uint32_t Timeout) //IIC从机发送数据
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
					if(IIC_Slave_Wait_Ack())  //IIC从机等待ACK信号	
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

