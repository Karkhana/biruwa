

#include"stddefs.h"

#define PM 1
#define AM 0

#define LCDClearLine1() LCDGotoXY(0,0);\
						LCDPrint("                ");\
						LCDGotoXY(0,0)

#define LCDClearLine2() LCDGotoXY(0,1);\
						LCDPrint("                ");\
						LCDGotoXY(0,1)

#define LCDBlinkOn() 	LCDCmd(0b00001111)
#define LCDBlinkOff() 	LCDCmd(0b00001100)



struct TIME
{
 uint8_t hr,min,sec;
};

volatile struct TIME curTime,resetTime;

uint8_t previousSecond=61;
uint16_t timeFresher=0;

void setTime(uint8_t hr,uint8_t min,uint8_t sec,uint8_t isPm);
void getTime(uint8_t *hr,uint8_t *min,uint8_t *sec);

void initKeypad()
{
 PORTA=0xFF;
 DDRA=0xFF;
}

uint8_t readKeypad()
{
 uint8_t i,j;
 uint16_t timeout;

 for(i=0;i<4;i++)
  {
   PORTA&=~(1<<i);
   for(j=0;j<4;j++)
    {
	 timeout=0;
	 DDRA&=~(1<<(4+j));

	 if(!(PINA&(1<<(4+j))))
	  {
	   _delay_ms(20);
	   while((!(PINA&(1<<(4+j)))) && timeout<1000);
	    {
		 _delay_ms(1);
	     timeout++;
		}
	   
	   DDRA|=(1<<(4+j));

	   if(timeout>=1000)
	    return 0;

	   _delay_ms(80);

	   return (i*4+j+1);
	  }
	}
	PORTA|=(1<<i);
  }

  return 0;
}


void displayTime()
{
  if(curTime.sec!=previousSecond)
    {
	 LCDGotoXY(0,0);
	 LCDPrint("Time:");
	 LCDWriteInt(curTime.hr);
	 LCDPrint(":");
	 LCDWriteInt(curTime.min);
	 LCDPrint(":");
	 LCDWriteInt(curTime.sec);

	 previousSecond=curTime.sec;
	}
}

int main()
{
 uint8_t keyData,dataChanged=0;
 uint8_t okCounter=0;
 
 DDRC=0b00000100;
 DDRD=0xFF;
 DDRB=0xFF;
 
 TCCR0=(1<<CS00);
 TIMSK=(1<<TOIE0);
 sei();

 initLCD();
 _delay_ms(50);
 LCDGotoXY(0,0);
 initKeypad();
 
 resetTime.hr=resetTime.min=resetTime.sec=0;

 DS1307Read(0x00,(uint8_t*)&curTime.sec);
 curTime.sec=curTime.sec&0b01111111;
 DS1307Write(0x00,curTime.sec);

 while(1)
  {
   	
	if((keyData=readKeypad()))
	 {
	  //key 1-- left
	  //key 2-- right
	  //key 3-- up
	  //key 4-- down
	  //key 5--OK

	  if(keyData==5)
	   {

		cli();

	    if(okCounter==0)
		 {
		  LCDClearLine1();
		  LCDPrint("Set Time");
		  LCDClearLine2();
		  LCDPrint("00-00-00");
		  //while(!(keyData=readKeypad()));
		  
		  while(1)
		  {
		   
		   while(!(keyData=readKeypad()));

		   if(keyData==5)
		    {
			 if(dataChanged==1)
			  {
		       setTime(resetTime.hr,resetTime.min,resetTime.sec,PM);
			  }
			  
			  goto last;
		    }
			
			if(keyData==3)
			 {
			  dataChanged=1;
			  resetTime.hr++;
			  if(resetTime.hr>12)
			   resetTime.hr=0;
			  
			  LCDGotoXY(0,1);
			  if(resetTime.hr<10)
			   {
			    LCDData(48);
				LCDData(48+resetTime.hr);
			   }
			  else
			   {
			    LCDData(resetTime.hr/10+48);
				LCDData(resetTime.hr%10+48);
			   }
			 }
		  }
		 }
		else if(okCounter==1)
		 {
		  LCDClearLine1();
		  LCDPrint("Set Strip1 Time");
		  LCDClearLine2();
		  LCDPrint("00-00-00");
		 }
		else if(okCounter==2)
		 {
		  LCDClearLine1();
		  LCDPrint("Set Strip2 Time");
		  LCDClearLine2();
		  LCDPrint("00-00-00");
		 }
		else if(okCounter==3)
		 {
		  LCDClear();
		  sei();
		 }
		
	 last:
	    dataChanged=0;
		okCounter++;
		if(okCounter>3)
		 okCounter=0;

	   }
	 }
  }
 return 0;
}

ISR(TIMER0_OVF_vect)
{
 timeFresher++;

 if(timeFresher>31250)
  {
   cli();
   timeFresher=0;
   getTime((uint8_t*)&curTime.hr,(uint8_t*)&curTime.min,(uint8_t*)&curTime.sec);
   displayTime();
   sei();
  }
}

void setTime(uint8_t hr,uint8_t min,uint8_t sec,uint8_t isPm)
{
 uint8_t hrConv,minConv,secConv;
 
 DS1307Write(0x00,0x80); //disable crystal first

 hrConv=0b01000000; //make 12 hour format

 if(isPm)
  hrConv|=0b0010000;
 
 
 hrConv=((hr/10)<<4)|(hr%10);
 minConv=((min/10)<<4)|(min%10);
 secConv=((sec/10)<<4)|(sec%10);

 secConv&=(~0x80);

 DS1307Write(0x02,hrConv);
 DS1307Write(0x01,minConv);
 DS1307Write(0x00,secConv);

}

void getTime(uint8_t *hr,uint8_t *min,uint8_t *sec)
{
 
 uint8_t temp;

 DS1307Read(0x00,sec);
 DS1307Read(0x01,min);
 DS1307Read(0x02,hr);
 
 temp=*sec;
 temp=(((temp&0b01110000)>>4)*10)+(temp&0x0F);
 *sec=temp;

 temp=*min;
 temp=(((temp&0b01110000)>>4)*10)+(temp&0x0F);
 *min=temp;

 temp=*hr;
 temp=(((temp&0b00010000)>>4)*10)+(temp&0x0F);
 *hr=temp;

}
