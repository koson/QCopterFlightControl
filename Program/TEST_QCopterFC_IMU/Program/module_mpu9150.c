/*==============================================================================================*/
/*==============================================================================================*/
#include "stm32f4_system.h"
#include "stm32f4_i2c.h"
#include "module_ak8975.h"
#include "module_mpu6050.h"
#include "module_mpu9150.h"
/*==============================================================================================*/
/*==============================================================================================*
**函數 : MPU9150_Init
**功能 : 初始化 MPU9150
**輸入 : None
**輸出 : None
**使用 : MPU9150_Init()
**==============================================================================================*/
/*==============================================================================================*/
u8 MPU9150_Init( void )
{
  u8 i = 0;
  u8 ReadID = 0;
  u8 MPU6050_Init_Data[6][2] = {
      {0x01, MPU6050_PWR_MGMT_1},   // Reset device
      {0x03, MPU6050_CONFIG},       // 
      {0x18, MPU6050_GYRO_CONFIG},  // +-2000dps
      {0x08, MPU6050_ACCEL_CONFIG}, // +-4G
      {0x32, MPU6050_INT_PIN_CFG},  // 
      {0x00, MPU6050_USER_CTRL}     // 
    };
  u8 AK8975_Init_Data[1][2] = {
      {0x01, AK8975_CNTL},   // 
    };

  /* MPU6050 */
  I2C_DMA_ReadReg(MPU6050_I2C_ADDR,  MPU6050_WHO_AM_I, &ReadID, 1);
  if(ReadID != MPU6050_Device_ID)
    return ERROR;

  Delay_1ms(10);

  for(i=0; i<6; i++) {
    I2C_DMA_WriteReg(MPU6050_I2C_ADDR, MPU6050_Init_Data[i][1], MPU6050_Init_Data[i], 1);
    Delay_1ms(10);
  }

  /* AK8975 */
  I2C_DMA_ReadReg(AK8975_I2C_ADDR,  AK8975_WIA, &ReadID, 1);
  if(ReadID != AK8975_Device_ID)
    return ERROR;

  for(i=0; i<1; i++) {
    I2C_DMA_WriteReg(AK8975_I2C_ADDR, AK8975_Init_Data[i][1], AK8975_Init_Data[i], 1);
    Delay_1ms(10);
  }
//  for(i=0; i<3; i++) {
//    I2C_DMA_WriteReg(AK8975_I2C_ADDR, AK8975_Init_Data[i][1], AK8975_Init_Data[i], 1);
//    Delay_1ms(10);
//  }

  return SUCCESS;
}
/*==============================================================================================*/
/*==============================================================================================*
**函數 : MPU9150_Read
**功能 : 初始化 MPU9150
**輸入 : None
**輸出 : None
**使用 : MPU9150_Read(IMU_Buf);
**==============================================================================================*/
/*==============================================================================================*/
void AK8975_Cal( u8* ReadBuf )
{
  u8 ReadData = 0x00;
  u8 WriteData = 0x01;
  static u8 begin_8975=1;

  if(begin_8975) {
    begin_8975 = 0;
    I2C_DMA_WriteReg(AK8975_I2C_ADDR, AK8975_CNTL, &WriteData, 1);
  }
  else {
    I2C_DMA_ReadReg(AK8975_I2C_ADDR, AK8975_ST1, &ReadData, 1);
    if(ReadData == 1) {
      I2C_DMA_ReadReg(AK8975_I2C_ADDR, AK8975_HXL, ReadBuf, 6);
      I2C_DMA_WriteReg(AK8975_I2C_ADDR, AK8975_CNTL, &WriteData, 1);
    }
  }
}
void MPU9150_Read( u8* ReadBuf )
{
  u8 ReadData = 0x00;
  u8 WriteData = 0x01;

  I2C_DMA_ReadReg(MPU6050_I2C_ADDR, MPU6050_ACCEL_XOUT_H, ReadBuf, 14);

  I2C_DMA_ReadReg(AK8975_I2C_ADDR, AK8975_ST1, &ReadData, 1);
  if(ReadData == 1) {
    I2C_DMA_ReadReg(AK8975_I2C_ADDR, AK8975_HXL, ReadBuf+14, 6);
    I2C_DMA_WriteReg(AK8975_I2C_ADDR, AK8975_CNTL, &WriteData, 1);
  }
}
/*==============================================================================================*/
/*==============================================================================================*
**函數 : MPU9150_Bypass
**功能 : MPU9150 Bypass Enable/Disable
**輸入 : None
**輸出 : None
**使用 : MPU9150_Bypass(1);
**==============================================================================================*/
/*==============================================================================================*/
void MPU9150_Bypass( u8 Bypass )
{
  u8 ReadBuf;

  if( Bypass ) {
    I2C_DMA_ReadReg(MPU6050_I2C_ADDR, MPU6050_USER_CTRL, &ReadBuf, 1);
    ReadBuf &= (~0x02);
    I2C_DMA_WriteReg(MPU6050_I2C_ADDR, MPU6050_USER_CTRL, &ReadBuf, 1);
    Delay_1ms(5);
    ReadBuf = 0x82;
    I2C_DMA_WriteReg(MPU6050_I2C_ADDR, MPU6050_INT_PIN_CFG, &ReadBuf, 1);
  }
  else {
    I2C_DMA_ReadReg(MPU6050_I2C_ADDR, MPU6050_USER_CTRL, &ReadBuf, 1);
    ReadBuf |= 0x02;
    I2C_DMA_WriteReg(MPU6050_I2C_ADDR, MPU6050_USER_CTRL, &ReadBuf, 1);
    Delay_1ms(5);
  }
}
/*==============================================================================================*/
/*==============================================================================================*/
