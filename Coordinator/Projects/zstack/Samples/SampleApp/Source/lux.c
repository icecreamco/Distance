#include <ioCC2530.h>
#include "OnBoard.h"

#define   uchar unsigned char
#define   uint unsigned int	
#define	  SCL P1_2      //IICʱ�����Ŷ���
#define   SDA P1_3      //IIC�������Ŷ���
#define	  SlaveAddress   0x46 //����������IIC�����еĴӵ�ַ,����ALT  ADDRESS��ַ���Ų�ͬ�޸�
                              //ALT  ADDRESS���Žӵ�ʱ��ַΪ0x46���ӵ�Դʱ��ַΪ0xB8

uchar    BUF[3];                         //�������ݻ�����      	
uchar   value_lux[5]="12345";            //��ʾ����
uint     dis_data;                       //����

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
    SDA = 1;                    //����������
    SCL = 1;                    //����ʱ����
    delayms(1);                 //��ʱ
    SDA = 0;                    //�����½���
    delayms(1);                 //��ʱ
    SCL = 0;                    //����ʱ����
    delayms(1);
}

void Stop(void)
{
    SDA = 0;                    //����������
    SCL = 1;                    //����ʱ����
    delayms(1);                //��ʱ
    SDA = 1;                    //����������
    delayms(1);                 //��ʱ
}

void SendACK(uchar ack)
{
    SDA = ack;                  //дӦ���ź�
    SCL = 1;                    //����ʱ����
    delayms(1);                 //��ʱ
    SCL = 0;                    //����ʱ����
    delayms(1);                 //��ʱ
}

void RecvACK(void)
{
    SCL = 1;                    //����ʱ����
    delayms(1);                 //��ʱ
    SCL = 0;                    //����ʱ����

}

void SendByte(uchar dat)
{
    uchar i;
    for (i=0; i<8; i++)         //8λ������
    {
	if(dat & 0x80)
          SDA = 1;
	else
          SDA = 0;
        dat <<= 1;              //�Ƴ����ݵ����λ
        SCL = 1;                //����ʱ����       
        delayms(1);             //��ʱ
        SCL = 0;                //����ʱ����
    }
    RecvACK();
}

uchar RecvByte(void)
{
    uchar i;
    uchar dat = 0;

    SDA = 1;                    //ʹ���ڲ�����,׼����ȡ����,
    P1DIR &= ~0x08;
    delayms(1);
    for (i=0; i<8; i++)         //8λ������
    {
        dat <<= 1;
        SCL = 1;                //����ʱ����
        delayms(1);             //��ʱ
        dat |= SDA;             //������   
        delayms(1);
        SCL = 0;                //����ʱ����
    }
    P1DIR |= 0x08;
    return dat;
}


void conversion(uint temp_data)  
{  
    value_lux[0]=temp_data/10000+0x30 ;
    if(value_lux[0] == '0')
        value_lux[0] = ' ';
    temp_data=temp_data%10000;   //ȡ������
    value_lux[1]=temp_data/1000+0x30 ;
    if(value_lux[1] == '0')
        value_lux[1] = ' ';
    temp_data=temp_data%1000;    //ȡ������
    value_lux[2]=temp_data/100+0x30   ;
    if(value_lux[2] == '0')
        value_lux[2] = ' ';
    temp_data=temp_data%100;     //ȡ������
    value_lux[3]=temp_data/10+0x30    ;
    if(value_lux[3] == '0')
        value_lux[3] = ' ';
    temp_data=temp_data%10;      //ȡ������
    value_lux[4]=temp_data+0x30; 	

}

void Single_Write(uchar REG_Address)
{
    Start();                  //��ʼ�ź�
    SendByte(SlaveAddress);   //�����豸��ַ+д�ź�
    SendByte(REG_Address);    //�ڲ��Ĵ�����ַ��
    Stop();                   //����ֹͣ�ź�
}

void Multiple_Read(void)
{   uchar i;	
    Start();                          //��ʼ�ź�
    SendByte(SlaveAddress+1);         //�����豸��ַ+���ź�
    for (i=0; i<2; i++)                      //������ȡ2����ַ���ݣ��洢��BUF
    {
        BUF[i] = RecvByte();          //BUF[0]�洢0x32��ַ�е�����
        if (i == 1)
        {
           SendACK(1);                //���һ��������Ҫ��NOACK
        }
        else
        {		
           SendACK(0);                //��ӦACK
       }
   }
    Stop();                          //ֹͣ�ź�
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
    delayms(10);              //��ʱ180ms
    Multiple_Read();       //�����������ݣ��洢��BUF��
    dis_data=BUF[0];
    dis_data=(dis_data<<8)+BUF[1];//�ϳ����ݣ�����������   
    temp=(float)dis_data/1.2;
    conversion(temp);         //�������ݺ���ʾ
}
    
    

