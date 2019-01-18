#ifndef __iic_ms_H
#define __iic_ms_H
#ifdef __cplusplus
 extern "C" {
#endif
#include "stm32f1xx_hal.h"
#include "main.h"
	
#define  IIC_SCL_H    HAL_GPIO_WritePin(GPIOB, IIC_SCL_Pin, GPIO_PIN_SET)
#define  IIC_SCL_L    HAL_GPIO_WritePin(GPIOB, IIC_SCL_Pin, GPIO_PIN_RESET)
#define  IIC_SDA_H    HAL_GPIO_WritePin(GPIOB, IIC_SDA_Pin, GPIO_PIN_SET)
#define  IIC_SDA_L    HAL_GPIO_WritePin(GPIOB, IIC_SDA_Pin, GPIO_PIN_RESET) 
#define  READ_SDA    HAL_GPIO_ReadPin(GPIOB, IIC_SDA_Pin)
#define  READ_SCL    HAL_GPIO_ReadPin(GPIOB, IIC_SCL_Pin)
#define  IIC_Address	   0x25
#define  IIC_Delay       delay_us(4)    //IIC使用的延时，可通过改变延时，改变IIC传输速率
#define  Time_Count      250           //while等待超时计数,不可小于某个值，	最小值根据单片机时钟变化

void delay_us(uint32_t nus);
void delay_ms(uint16_t nms);
	 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
void IIC_Slave_Send_Byte(uint8_t txd); 	//IIC从机发送一个字节 
uint8_t IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
uint8_t IIC_Slave_Read_Byte(unsigned char ack);//IIC从机读取一个字节	 
uint8_t IIC_Wait_Ack(void); 				//IIC等待ACK信号 
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC发送NACK信号
uint8_t IIC_Slave_Wait_Ack(void);	
void IIC_Slave_Ack(void);   //从机发送ACK信号
void IIC_Slave_NAck(void);  //从机发送NACK信号
uint8_t wait_Start(void);	  
uint8_t wait_Stop(void);	
uint8_t I2C_Master_Transmit( uint8_t DevAddress,uint8_t *pData, uint16_t Size); //主机向特定从机发送数据
uint8_t I2C_Master_Receive( uint8_t DevAddress,uint8_t *pData, uint16_t Size);  //主机从特定从机接收数据
uint8_t I2C_Slave_Transmit( uint8_t *pData, uint16_t Size, uint32_t Timeout);  //从机发送数据      
uint8_t I2C_Slave_Receive( uint8_t *pData, uint16_t Size, uint32_t Timeout);  //从机接收数据


#endif 
