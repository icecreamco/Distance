#include <ioCC2530.h>
#include "OnBoard.h"

typedef unsigned char uchar;
typedef unsigned int  uint;

#define DATA_PIN P0_7

void Delay_us(void);
void Delay_10us(void);
void Delay_ms(uint Time);
void COM(void);
void DHT11(void);

//��ʪ�ȶ���
uchar ucharFLAG,uchartemp;
uchar ucharT_data_H,ucharT_data_L,ucharRH_data_H,ucharRH_data_L,ucharcheckdata;
uchar ucharT_data_H_temp,ucharT_data_L_temp,ucharRH_data_H_temp,ucharRH_data_L_temp,ucharcheckdata_temp;
uchar ucharcomdata;
uint  T_temp,RH_temp;
uchar value_wenshi[8];

//��ʱ����
void Delay_us(void) //1 us��ʱ
{
    MicroWait(1);   
}

void Delay_10us(void) //10 us��ʱ
{
   MicroWait(10);
}

void Delay_ms(uint Time)//n ms��ʱ
{
  unsigned char i;
  while(Time--)
  {
    for(i=0;i<100;i++)
     Delay_10us();
  }
}

//��ʪ�ȴ���
void COM(void)    // ��ʪд��
{     
    uchar i;         
    for(i=0;i<8;i++)    
    {
        ucharFLAG=2; 
        while((!DATA_PIN)&&ucharFLAG++);
        Delay_10us();
        Delay_10us();
        Delay_10us();
        uchartemp=0;
        if(DATA_PIN)uchartemp=1;
        ucharFLAG=2;
        while((DATA_PIN)&&ucharFLAG++);   
        if(ucharFLAG==1)break;    
        ucharcomdata<<=1;
        ucharcomdata|=uchartemp; 
    }    
}

void DHT11(void)   //��ʪ��������
{
    DATA_PIN=0;
    Delay_ms(19);  //>18MS
    DATA_PIN=1; 
    P0DIR &= ~0x80; //��������IO�ڷ���
    Delay_10us();
    Delay_10us();                        
    Delay_10us();
    Delay_10us();  
    if(!DATA_PIN) 
    {
        ucharFLAG=2; 
        while((!DATA_PIN)&&ucharFLAG++);
        ucharFLAG=2;
        while((DATA_PIN)&&ucharFLAG++); 
        COM();
        ucharRH_data_H_temp=ucharcomdata;
        COM();
        ucharRH_data_L_temp=ucharcomdata;
        COM();
        ucharT_data_H_temp=ucharcomdata;
        COM();
        ucharT_data_L_temp=ucharcomdata;
        COM();
        ucharcheckdata_temp=ucharcomdata;
        DATA_PIN=1; 
        uchartemp=(ucharT_data_H_temp+ucharT_data_L_temp+ucharRH_data_H_temp+ucharRH_data_L_temp);
        if(uchartemp==ucharcheckdata_temp)
        {
            ucharRH_data_H=ucharRH_data_H_temp;
            ucharRH_data_L=ucharRH_data_L_temp;
            ucharT_data_H=ucharT_data_H_temp;
            ucharT_data_L=ucharT_data_L_temp;
            ucharcheckdata=ucharcheckdata_temp;
        }
        if(ucharT_data_H & 0x80)
            value_wenshi[0] = '-';
        else
            value_wenshi[0] = ' ';
        ucharT_data_H &= ~0x80;
        T_temp = ucharT_data_H*256+ucharT_data_L;
        RH_temp = ucharRH_data_H*256+ucharRH_data_L;
        
        value_wenshi[1]=T_temp/100+0x30; 
        value_wenshi[2]=T_temp%100/10+0x30;
        value_wenshi[3]='.';
        value_wenshi[4]=T_temp%10+0x30;      
        value_wenshi[5]=RH_temp/100+0x30;  
        value_wenshi[6]=RH_temp%100/10+0x30;       
    } 
    else //û�óɹ���ȡ������0
    {
//        wendu_shi=0; 
//        wendu_ge=0;
//        
//        shidu_shi=0; 
//        shidu_ge=0;  
    } 
    
    P0DIR |= 0x80; //IO����Ҫ�������� 
}
