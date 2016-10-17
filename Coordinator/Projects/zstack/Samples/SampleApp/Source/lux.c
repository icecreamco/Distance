#include <ioCC2530.h>
#include "OnBoard.h"

#define   uchar unsigned char
#define   uint unsigned int	
#define	  SCL P1_2      //IIC时钟引脚定义
#define   SDA P1_3      //IIC数据引脚定义
#define	  SlaveAddress   0x46 //定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改
                              //ALT  ADDRESS引脚接地时地址为0x46，接电源时地址为0xB8

uchar    BUF[3];                         //接收数据缓存区      	
uchar   value_lux[5]="12345";            //显示变量
uint     dis_data;                       //变量

void delayms(uchar i);
void Start(void);
void Stop(void);
void SendACK(uchar ack);
void RecvACK(void);
uchar RecvByte(void);
void SendByte(uchar dat);
uchar RecvByte(void);
void conversion(uint temp_data);
void Single_Write(uchar REG_Address);
void Multiple_Read(void);

void lux(void);
void conversion(uint temp_data);


void delayms(uchar i)
{
  for(;i>0;i--)
  {
     asm("NOP");
     asm("NOP");
     asm("NOP");
  }
}
void Start(void)
{
    SDA = 1;                    //拉高数据线
    SCL = 1;                    //拉高时钟线
    delayms(1);                 //延时
    SDA = 0;                    //产生下降沿
    delayms(1);                 //延时
    SCL = 0;                    //拉低时钟线
    delayms(1);
}

void Stop(void)
{
    SDA = 0;                    //拉低数据线
    SCL = 1;                    //拉高时钟线
    delayms(1);                //延时
    SDA = 1;                    //产生上升沿
    delayms(1);                 //延时
}

void SendACK(uchar ack)
{
    SDA = ack;                  //写应答信号
    SCL = 1;                    //拉高时钟线
    delayms(1);                 //延时
    SCL = 0;                    //拉低时钟线
    delayms(1);                 //延时
}

void RecvACK(void)
{
    SCL = 1;                    //拉高时钟线
    delayms(1);                 //延时
    SCL = 0;                    //拉低时钟线

}

void SendByte(uchar dat)
{
    uchar i;
    for (i=0; i<8; i++)         //8位计数器
    {
	if(dat & 0x80)
          SDA = 1;
	else
          SDA = 0;
        dat <<= 1;              //移出数据的最高位
        SCL = 1;                //拉高时钟线       
        delayms(1);             //延时
        SCL = 0;                //拉低时钟线
    }
    RecvACK();
}

uchar RecvByte(void)
{
    uchar i;
    uchar dat = 0;

    SDA = 1;                    //使能内部上拉,准备读取数据,
    P1DIR &= ~0x08;
    delayms(1);
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;
        SCL = 1;                //拉高时钟线
        delayms(1);             //延时
        dat |= SDA;             //读数据   
        delayms(1);
        SCL = 0;                //拉低时钟线
    }
    P1DIR |= 0x08;
    return dat;
}


void conversion(uint temp_data)  
{  
    value_lux[0]=temp_data/10000+0x30 ;
    if(value_lux[0] == '0')
        value_lux[0] = ' ';
    temp_data=temp_data%10000;   //取余运算
    value_lux[1]=temp_data/1000+0x30 ;
    if(value_lux[1] == '0')
        value_lux[1] = ' ';
    temp_data=temp_data%1000;    //取余运算
    value_lux[2]=temp_data/100+0x30   ;
    if(value_lux[2] == '0')
        value_lux[2] = ' ';
    temp_data=temp_data%100;     //取余运算
    value_lux[3]=temp_data/10+0x30    ;
    if(value_lux[3] == '0')
        value_lux[3] = ' ';
    temp_data=temp_data%10;      //取余运算
    value_lux[4]=temp_data+0x30; 	

}

void Single_Write(uchar REG_Address)
{
    Start();                  //起始信号
    SendByte(SlaveAddress);   //发送设备地址+写信号
    SendByte(REG_Address);    //内部寄存器地址，
    Stop();                   //发送停止信号
}

void Multiple_Read(void)
{   uchar i;	
    Start();                          //起始信号
    SendByte(SlaveAddress+1);         //发送设备地址+读信号
    for (i=0; i<2; i++)                      //连续读取2个地址数据，存储中BUF
    {
        BUF[i] = RecvByte();          //BUF[0]存储0x32地址中的数据
        if (i == 1)
        {
           SendACK(1);                //最后一个数据需要回NOACK
        }
        else
        {		
           SendACK(0);                //回应ACK
       }
   }
    Stop();                          //停止信号
}


void lux(void)
{  
   float temp;
   P1DIR |= 0x8c;
   P1SEL &= ~0x8c;
   P1INP &= ~0x8c;
   
   P0DIR |= 0x01;
   P0SEL &= ~0x01;
   P0INP &= ~0x01;
   delayms(1);
   P1_7 = 0;
   P0_0 = 0;
    
   Single_Write(0x01);
   delayms(1);
     
    Single_Write(0x01);   // power on
    Single_Write(0x10);   // H- resolution mode
    delayms(10);              //延时180ms
    Multiple_Read();       //连续读出数据，存储在BUF中
    dis_data=BUF[0];
    dis_data=(dis_data<<8)+BUF[1];//合成数据，即光照数据   
    temp=(float)dis_data/1.2;
    conversion(temp);         //计算数据和显示
}
    
    

