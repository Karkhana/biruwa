
#include"stddefs.h"
#include"lcd.h"

void initLCD(void)
{

 LCDCmd(0x38);	 // initialization of 16X2 LCD in 8bit mode
 _delay_ms(1);

 LCDCmd(0x01);	 // clear LCD
 _delay_ms(1);

 LCDCmd(0x0C);	 // cursor ON
 _delay_ms(1);

 LCDCmd(0x80);	 // ---8 go to first line and --0 is for 0th position
 _delay_ms(1);

}

void LCDCmd(unsigned char cmd)
{
LCD_DATA=cmd;
ctrl =(0<<rs)|(0<<rw)|(1<<en);	
_delay_ms(1);
ctrl =(0<<rs)|(0<<rw)|(0<<en);	
_delay_ms(50);
}

void LCDData(unsigned char data)
{
LCD_DATA= data;
ctrl = (1<<rs)|(0<<rw)|(1<<en);
_delay_ms(1);
ctrl = (1<<rs)|(0<<rw)|(0<<en);
_delay_ms(5);	
}

void LCDPrint(char *str)	
{
 unsigned int i=0;
 while(str[i]!='\0')	  // loop will go on till the NULL character in the string 
  {
    LCDData(str[i]);	 // sending data on LCD byte by byte
    i++;
  }
}

void LCDClear()
{
 LCDCmd(0x01);
}

void LCDGotoXY(uint8_t x,uint8_t y)
{

 uint8_t address=0x80;

 if(y==1)
  address=0xC0;

  address+=x;

  LCDCmd(address);
}

void LCDSmartWrite(int x,int y,char *str)
{
 int j=0;

 while(str[j])
 {
  if(x>15)
   {
    x=0;
	y++;
   }
      
  LCDGotoXY(x,y);
  LCDData(str[j]);
  j++;
  x++;
 }

}

void LCDWriteInt(int16_t data)
{
 	uint8_t num[5],i=0;
	
	if(data==0)
	 LCDPrint("00");
    else if(data/10==0)
	 {
	  LCDPrint("0");
	  LCDData(data+48);
	 }
	else
	 {
	while(data)
	 {
	  num[i++]=data%10;
	  data/=10;
	 }
	  
	 for(;i>0;i--)
	   LCDData(num[i-1]+'0');
	}
}
