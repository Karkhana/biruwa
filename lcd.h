

#define LCD_DATA PORTB	 //LCD data port
#define ctrl PORTD
#define en PD5	 // enable signal
#define rw PD4	 // read/write signal
#define rs PD3	 // register select signal

#define D1 PC0
#define D2 PC1
#define D3 PC2
#define D4 PC3
#define D5 PC4
#define D6 PC5
#define D7 PC6

void LCDCmd(unsigned char cmd);
void initLCD(void);
void LCDData(unsigned char data);
void LCDPrint(char *str);
void LCDClear();
void LCDGotoXY(uint8_t x,uint8_t y);
void LCDSmartWrite(int x,int y,char *str);
void LCDWriteInt(int16_t data);
