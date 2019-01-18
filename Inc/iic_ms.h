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
#define  IIC_Delay       delay_us(4)    //IICʹ�õ���ʱ����ͨ���ı���ʱ���ı�IIC��������
#define  Time_Count      250           //while�ȴ���ʱ����,����С��ĳ��ֵ��	��Сֵ���ݵ�Ƭ��ʱ�ӱ仯

void delay_us(uint32_t nus);
void delay_ms(uint16_t nms);
	 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
void IIC_Slave_Send_Byte(uint8_t txd); 	//IIC�ӻ�����һ���ֽ� 
uint8_t IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
uint8_t IIC_Slave_Read_Byte(unsigned char ack);//IIC�ӻ���ȡһ���ֽ�	 
uint8_t IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź� 
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC����NACK�ź�
uint8_t IIC_Slave_Wait_Ack(void);	
void IIC_Slave_Ack(void);   //�ӻ�����ACK�ź�
void IIC_Slave_NAck(void);  //�ӻ�����NACK�ź�
uint8_t wait_Start(void);	  
uint8_t wait_Stop(void);	
uint8_t I2C_Master_Transmit( uint8_t DevAddress,uint8_t *pData, uint16_t Size); //�������ض��ӻ���������
uint8_t I2C_Master_Receive( uint8_t DevAddress,uint8_t *pData, uint16_t Size);  //�������ض��ӻ���������
uint8_t I2C_Slave_Transmit( uint8_t *pData, uint16_t Size, uint32_t Timeout);  //�ӻ���������      
uint8_t I2C_Slave_Receive( uint8_t *pData, uint16_t Size, uint32_t Timeout);  //�ӻ���������


#endif 
