/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2020,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：三群：824575535
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file            ICM20602
 * @company         成都逐飞科技有限公司
 * @author          逐飞科技(QQ3184284598)
 * @version         查看doc内version文件 版本说明
 * @Software        ADS v1.2.2
 * @Target core     TC364DP
 * @Taobao          https://seekfree.taobao.com/
 * @date            2020-11-23
 * @note
                    接线定义：
                    ------------------------------------
                        SCL                 查看SEEKFREE_IIC文件内的SEEKFREE_SCL宏定义
                        SDA                 查看SEEKFREE_IIC文件内的SEEKFREE_SDA宏定义
                    ------------------------------------
 ********************************************************************************************************************/


#include "zf_stm_systick.h"
#include "zf_gpio.h"
#include "zf_spi.h"
#include "SEEKFREE_IIC.h"
#include "SEEKFREE_ICM20602.h"
#include "HCI.h"

int16 icm_gyro_x,icm_gyro_y,icm_gyro_z;
int16 icm_acc_x,icm_acc_y,icm_acc_z;

int16 OffsetX;
int16 OffsetY;
int16 OffsetZ = 0;

float Accal_x; //ICM20602加速度计数据解算后的数据
float Accal_y;
float Accal_z;

float Gyro_x;
float Gyro_y;
float Gyro_z;

float I_angle;
char start_angle_i_flag;


//-------------------------------------------------------------------------------------------------------------------
//  @brief      ICM20602自检函数
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:               调用该函数前，请先调用模拟IIC的初始化
//-------------------------------------------------------------------------------------------------------------------
//void icm20602_self1_check(void)
//{
//    uint8 dat=0;
//    while(0x12 != dat)   //读取ICM20602 ID
//    {
//        dat = simiic_read_reg(ICM20602_DEV_ADDR,ICM20602_WHO_AM_I,SIMIIC);
//        systick_delay_ms(STM0, 10);
//        //卡在这里原因有以下几点
//        //1 ICM20602坏了，如果是新的这样的概率极低
//        //2 接线错误或者没有接好
//        //3 可能你需要外接上拉电阻，上拉到3.3V
//      //4 可能没有调用模拟IIC的初始化函数
//    }
//}



//-------------------------------------------------------------------------------------------------------------------
//  @brief      初始化ICM20602
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:               调用该函数前，请先调用模拟IIC的初始化
//-------------------------------------------------------------------------------------------------------------------
//void icm20602_init(void)
//{
//  simiic_init();
//    systick_delay_ms(STM0, 10);  //上电延时
//
//    //检测
//    icm20602_self1_check();
//
//    //复位
//    simiic_write_reg(ICM20602_DEV_ADDR,ICM20602_PWR_MGMT_1,0x80);               //复位设备
//    systick_delay_ms(STM0, 2);                                                        //延时
//    while(0x80 & simiic_read_reg(ICM20602_DEV_ADDR,ICM20602_PWR_MGMT_1,SIMIIC));//等待复位完成
//
//    //配置参数
//    simiic_write_reg(ICM20602_DEV_ADDR,ICM20602_PWR_MGMT_1,0x01);               //时钟设置
//    simiic_write_reg(ICM20602_DEV_ADDR,ICM20602_PWR_MGMT_2,0x00);               //开启陀螺仪和加速度计
//    simiic_write_reg(ICM20602_DEV_ADDR,ICM20602_CONFIG,0x01);                   //176HZ 1KHZ
//    simiic_write_reg(ICM20602_DEV_ADDR,ICM20602_SMPLRT_DIV,0x07);               //采样速率 SAMPLE_RATE = INTERNAL_SAMPLE_RATE / (1 + SMPLRT_DIV)
//    simiic_write_reg(ICM20602_DEV_ADDR,ICM20602_GYRO_CONFIG,0x18);              //±2000 dps
//    simiic_write_reg(ICM20602_DEV_ADDR,ICM20602_ACCEL_CONFIG,0x10);             //±8g
//    simiic_write_reg(ICM20602_DEV_ADDR,ICM20602_ACCEL_CONFIG_2,0x03);           //Average 4 samples   44.8HZ   //0x23 Average 16 samples
//}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      获取ICM20602加速度计数据
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:               执行该函数后，直接查看对应的变量即可
//-------------------------------------------------------------------------------------------------------------------
//void get_icm20602_accdata(void)
//{
//    uint8 dat[6];
//
//    simiic_read_regs(ICM20602_DEV_ADDR, ICM20602_ACCEL_XOUT_H, dat, 6, SIMIIC);
//    icm_acc_x = (int16)(((uint16)dat[0]<<8 | dat[1]));
//    icm_acc_y = (int16)(((uint16)dat[2]<<8 | dat[3]));
//    icm_acc_z = (int16)(((uint16)dat[4]<<8 | dat[5]));
//}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      获取ICM20602陀螺仪数据
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:               执行该函数后，直接查看对应的变量即可
//-------------------------------------------------------------------------------------------------------------------
//void get_icm20602_gyro(void)
//{
//    uint8 dat[6];
//
//    simiic_read_regs(ICM20602_DEV_ADDR, ICM20602_GYRO_XOUT_H, dat, 6, SIMIIC);
//    icm_gyro_x = (int16)(((uint16)dat[0]<<8 | dat[1]));
//    icm_gyro_y = (int16)(((uint16)dat[2]<<8 | dat[3]));
//    icm_gyro_z = (int16)(((uint16)dat[4]<<8 | dat[5]));
//}

//-------------------------------------------------------------------------------------------------------------------
//  以上函数是使用软件IIC通信，相比较硬件IIC，软件IIC引脚更加灵活，可以使用任意普通IO
//-------------------------------------------------------------------------------------------------------------------





////-------------------------------------------------------------------------------------------------------------------
////  以下函数是使用硬件IIC通信，相比较软件IIC，硬件IIC速度可以做到更快。
////-------------------------------------------------------------------------------------------------------------------
//
//#define IIC_NUM         IIC_1
//#define IIC_SDA_PIN     IIC1_SDA_B17
//#define IIC_SCL_PIN     IIC1_SCL_B16
////-------------------------------------------------------------------------------------------------------------------
////  @brief      ICM20602自检函数
////  @param      NULL
////  @return     void
////  @since      v1.0
////  Sample usage:
////-------------------------------------------------------------------------------------------------------------------
//void icm20602_self2_check(void)
//{
//    uint8 dat=0;
//    while(0x12 != dat)   //读取ICM20602 ID
//    {
//        iic_read_reg(IIC_NUM, ICM20602_DEV_ADDR, ICM20602_WHO_AM_I, &dat);
//        systick_delay_ms(STM0, 10);
//        //卡在这里原因有以下几点
//        //1 ICM20602坏了，如果是新的这样的概率极低
//        //2 接线错误或者没有接好
//        //3 可能你需要外接上拉电阻，上拉到3.3V
//    }
//}
//
////-------------------------------------------------------------------------------------------------------------------
////  @brief      初始化ICM20602
////  @param      NULL
////  @return     void
////  @since      v1.0
////  Sample usage:
////-------------------------------------------------------------------------------------------------------------------
//void icm20602_init_hardware(void)
//{
//    uint8 dat = 0;
//    systick_delay_ms(STM0, 10);  //上电延时
//    iic_init(IIC_NUM, IIC_SDA_PIN, IIC_SCL_PIN,400*1000);       //硬件IIC初始化     波特率400K
//
//    //检测
//    icm20602_self2_check();
//
//    //复位
//    iic_write_reg(IIC_NUM,ICM20602_DEV_ADDR, ICM20602_PWR_MGMT_1, 0x80);          //解除休眠状态
//    systick_delay_ms(STM0, 2);                                                            //延时
//
//
//    while(0x80 & dat)                                                               //等待复位完成
//    {
//        iic_read_reg(IIC_NUM, ICM20602_DEV_ADDR, ICM20602_PWR_MGMT_1, &dat);
//        systick_delay_ms(STM0, 10);
//    }
//
//    //配置参数
//    iic_write_reg(IIC_NUM,ICM20602_DEV_ADDR, ICM20602_PWR_MGMT_1, 0x01);          //时钟设置
//    iic_write_reg(IIC_NUM,ICM20602_DEV_ADDR, ICM20602_PWR_MGMT_2, 0x00);            //开启陀螺仪和加速度计
//    iic_write_reg(IIC_NUM,ICM20602_DEV_ADDR, ICM20602_CONFIG, 0x01);                //176HZ 1KHZ
//    iic_write_reg(IIC_NUM,ICM20602_DEV_ADDR, ICM20602_SMPLRT_DIV, 0x07);            //采样速率 SAMPLE_RATE = INTERNAL_SAMPLE_RATE / (1 + SMPLRT_DIV)
//    iic_write_reg(IIC_NUM,ICM20602_DEV_ADDR, ICM20602_GYRO_CONFIG, 0x18);           //±2000 dps
//    iic_write_reg(IIC_NUM,ICM20602_DEV_ADDR, ICM20602_ACCEL_CONFIG, 0x10);          //±8g
//    iic_write_reg(IIC_NUM,ICM20602_DEV_ADDR, ICM20602_ACCEL_CONFIG_2, 0x03);      //Average 4 samples   44.8HZ   //0x23 Average 16 samples
//
//}
//
//
////-------------------------------------------------------------------------------------------------------------------
////  @brief      获取ICM20602加速度计数据
////  @param      NULL
////  @return     void
////  @since      v1.0
////  Sample usage:             执行该函数后，直接查看对应的变量即可
////-------------------------------------------------------------------------------------------------------------------
//void get_icm20602_accdata_hardware(void)
//{
//    uint8 dat[6];
//
//    iic_read_reg_bytes(IIC_NUM,ICM20602_DEV_ADDR, ICM20602_ACCEL_XOUT_H, dat, 6);
//    icm_acc_x = (int16)(((uint16)dat[0]<<8 | dat[1]));
//    icm_acc_y = (int16)(((uint16)dat[2]<<8 | dat[3]));
//    icm_acc_z = (int16)(((uint16)dat[4]<<8 | dat[5]));
//}
//
////-------------------------------------------------------------------------------------------------------------------
////  @brief      获取ICM20602陀螺仪数据
////  @param      NULL
////  @return     void
////  @since      v1.0
////  Sample usage:             执行该函数后，直接查看对应的变量即可
////-------------------------------------------------------------------------------------------------------------------
//void get_icm20602_gyro_hardware(void)
//{
//    uint8 dat[6];
//
//    iic_read_reg_bytes(IIC_NUM,ICM20602_DEV_ADDR, ICM20602_GYRO_XOUT_H, dat, 6);
//    icm_gyro_x = (int16)(((uint16)dat[0]<<8 | dat[1]));
//    icm_gyro_y = (int16)(((uint16)dat[2]<<8 | dat[3]));
//    icm_gyro_z = (int16)(((uint16)dat[4]<<8 | dat[5]));
//}






//-------------------------------------------------------------------------------------------------------------------
//  以下函数是使用硬件SPI通信，相比较IIC，速度比IIC快非常多。
//-------------------------------------------------------------------------------------------------------------------
#define SPI_NUM         SPI_0
#define SPI_SCK_PIN     SPI0_SCLK_P20_11    //接模块SPC
#define SPI_MOSI_PIN    SPI0_MOSI_P20_14    //接模块SDI
#define SPI_MISO_PIN    SPI0_MISO_P20_12    //接模块SDO
#define SPI_CS_PIN      SPI0_CS2_P20_13     //接模块CS

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ICM20602 SPI写寄存器
//  @param      cmd     寄存器地址
//  @param      val     需要写入的数据
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void icm_spi_w_reg_byte(uint8 cmd, uint8 val)
{
    uint8 dat[2];
    
    dat[0] = cmd | ICM20602_SPI_W;
    dat[1] = val;
    
    spi_mosi(SPI_NUM,SPI_CS_PIN,dat,dat,2,1);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ICM20602 SPI读寄存器
//  @param      cmd     寄存器地址
//  @param      *val    接收数据的地址
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void icm_spi_r_reg_byte(uint8 cmd, uint8 *val)
{
    uint8 dat[2];
    
    dat[0] = cmd | ICM20602_SPI_R;
    dat[1] = *val;
    
    spi_mosi(SPI_NUM,SPI_CS_PIN,dat,dat,2,1);
    
    *val = dat[1];
}
  
//-------------------------------------------------------------------------------------------------------------------
//  @brief      ICM20602 SPI多字节读寄存器
//  @param      cmd     寄存器地址
//  @param      *val    接收数据的地址
//  @param      num     读取数量
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void icm_spi_r_reg_bytes(uint8 * val, uint8 num)
{
    spi_mosi(SPI_NUM,SPI_CS_PIN,val,val,num,1);
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      ICM20602自检函数
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void icm20602_self3_check(void)
{
    uint8 dat=0;
    while(0x12 != dat)   //读取ICM20602 ID
    {
        icm_spi_r_reg_byte(ICM20602_WHO_AM_I,&dat);
        systick_delay_ms(STM0, 10);
        //卡在这里原因有以下几点
        //1 ICM20602坏了，如果是新的这样的概率极低
        //2 接线错误或者没有接好
        //3 可能你需要外接上拉电阻，上拉到3.3V
    }

}
     
//-------------------------------------------------------------------------------------------------------------------
//  @brief      初始化ICM20602
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void icm20602_init_spi(void)
{
    uint8 val = 0x0;

    systick_delay_ms(STM0, 10);  //上电延时
    
    (void)spi_init(SPI_NUM, SPI_SCK_PIN, SPI_MOSI_PIN, SPI_MISO_PIN, SPI_CS_PIN, 3, 10*1000*1000);//硬件SPI初始化

    icm20602_self3_check();//检测
    
    icm_spi_w_reg_byte(ICM20602_PWR_MGMT_1,0x80);//复位设备
    systick_delay_ms(STM0, 2);
    do
    {//等待复位成功
        icm_spi_r_reg_byte(ICM20602_PWR_MGMT_1,&val);
    }while(0x41 != val);
    
    icm_spi_w_reg_byte(ICM20602_PWR_MGMT_1,     0x01);            //时钟设置
    icm_spi_w_reg_byte(ICM20602_PWR_MGMT_2,     0x00);            //开启陀螺仪和加速度计
    icm_spi_w_reg_byte(ICM20602_CONFIG,         0x01);            //176HZ 1KHZ
    icm_spi_w_reg_byte(ICM20602_SMPLRT_DIV,     0x07);            //采样速率 SAMPLE_RATE = INTERNAL_SAMPLE_RATE / (1 + SMPLRT_DIV)
    icm_spi_w_reg_byte(ICM20602_GYRO_CONFIG,    0x18);            //±2000 dps
    icm_spi_w_reg_byte(ICM20602_ACCEL_CONFIG,   0x10);            //±8g
    icm_spi_w_reg_byte(ICM20602_ACCEL_CONFIG_2, 0x03);            //Average 4 samples   44.8HZ   //0x23 Average 16 samples
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      获取ICM20602加速度计数据
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:               执行该函数后，直接查看对应的变量即可
//-------------------------------------------------------------------------------------------------------------------
void get_icm20602_accdata_spi(void)
{
    struct
    {
        uint8 reg;
        uint8 dat[6];
    }buf;

    buf.reg = ICM20602_ACCEL_XOUT_H | ICM20602_SPI_R;
    
    icm_spi_r_reg_bytes(&buf.reg, 7);
    icm_acc_x = (int16)(((uint16)buf.dat[0]<<8 | buf.dat[1]));
    icm_acc_y = (int16)(((uint16)buf.dat[2]<<8 | buf.dat[3]));
    icm_acc_z = (int16)(((uint16)buf.dat[4]<<8 | buf.dat[5]));
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      获取ICM20602陀螺仪数据
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:               执行该函数后，直接查看对应的变量即可
//-------------------------------------------------------------------------------------------------------------------
void get_icm20602_gyro_spi(void)
{
    struct
    {
        uint8 reg;
        uint8 dat[6];
    }buf;

    buf.reg = ICM20602_GYRO_XOUT_H | ICM20602_SPI_R;
    
    icm_spi_r_reg_bytes(&buf.reg, 7);
    icm_gyro_x = (int16)(((uint16)buf.dat[0]<<8 | buf.dat[1]));
    icm_gyro_y = (int16)(((uint16)buf.dat[2]<<8 | buf.dat[3]));
    icm_gyro_z = (int16)(((uint16)buf.dat[4]<<8 | buf.dat[5]));
}


////-------------------------------------------------------------------------------------------------------------------
////@brief 陀螺仪零漂初始化
//// 通过采集一定数据求均值计算陀螺仪零点偏移值。
////后续 陀螺仪读取的数据 - 零飘值，即可去除零点偏移量。
////-------------------------------------------------------------------------------------------------------------------
//void gyroOffsetInit(void)
//{
////    OffsetX = 0;
////    OffsetY = 0;
//    OffsetZ = 0;
//
//    for (int8 i = 0; i < 100; ++i)
//    {
//        get_icm20602_gyro_spi();    // 获取陀螺仪角速度
////        OffsetX += icm_gyro_x;
////        OffsetY += icm_gyro_y;
//        OffsetZ += icm_gyro_z;
//        systick_delay_ms(STM0, 2);    // 最大 1Khz
//    }
//
////    OffsetX /= 100;
////    OffsetY /= 100;
//    OffsetZ /= 100;
//}


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//-------------------------------------------------------------------------------------------------------------------
// @brief 将采集的数值转化为实际物理值, 并对陀螺仪进行去零漂处理
//加速度计初始化配置 -> 测量范围: ±8g        对应灵敏度: 4096 LSB/g
//陀螺仪初始化配置   -> 测量范围: ±2000 dps  对应灵敏度: 16.4 LSB/dps   (degree per second)
//@tips: gyro = (gyro_val / 16.4) °/s = ((gyro_val / 16.4) * PI / 180) rad/s
 //-------------------------------------------------------------------------------------------------------------------
void icmGetValues(void)
{
    get_icm20602_accdata_spi();  //获取ICM20602加速度计数据
    get_icm20602_gyro_spi();  //获取ICM20602陀螺仪数据

//    //加速度计进行加速度解算（米每二次方秒）
    Accal_x = (9.7947 * icm_acc_x ) / 4096.0 ;
    Accal_y = (9.7947 * icm_acc_y )  / 4096.0 ;
    Accal_z = (9.7947 * icm_acc_z )  / 4096.0 ;

    //陀螺仪角速度转换为度每秒（°/s）
    Gyro_x = ((float) icm_gyro_x - OffsetX) / 16.4;  //传感器绕 X 轴旋转的速度
    Gyro_y = ((float) icm_gyro_y - OffsetY) / 16.4;  //传感器绕 Y 轴旋转的速度
    if(icm_gyro_z<=10 && icm_gyro_z>=-10)
    {
        icm_gyro_z = 0;
    }
    Gyro_z = ((float) icm_gyro_z - OffsetZ) / 16.4;  //传感器绕 Z 轴旋转的速度

    //基于陀螺仪的角度积分
    if(start_angle_i_flag == 1)
    {
        I_angle += (Gyro_z * 0.01);   //10ms
    }
//    float alpha = 0.3;
//
//    //一阶低通滤波，单位g
//    icm_data.acc_x = (((float) icm_acc_x) * alpha) / 4096 + icm_data.acc_x * (1 - alpha);
//    icm_data.acc_y = (((float) icm_acc_y) * alpha) / 4096 + icm_data.acc_y * (1 - alpha);
//    icm_data.acc_z = (((float) icm_acc_z) * alpha) / 4096 + icm_data.acc_z * (1 - alpha);
//
//    //! 陀螺仪角速度必须转换为弧度制角速度: deg/s -> rad/s
//    icm_data.gyro_x = ((float) icm_gyro_x - GyroOffset.Xdata) * PI / 180 / 16.4f;
//    icm_data.gyro_y = ((float) icm_gyro_y - GyroOffset.Ydata) * PI / 180 / 16.4f;
//    icm_data.gyro_z = ((float) icm_gyro_z - GyroOffset.Zdata) * PI / 180 / 16.4f;
}
