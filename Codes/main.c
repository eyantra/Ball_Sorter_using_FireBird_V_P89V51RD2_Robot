#include<intrins.h>
#include"p89v51rx2.h"
#include"lcd.h"

sbit touch_sensor=P1^6; // Touch Sensor Connected to Pin 1.6

sbit start_conv = P3^6;		 //P3^6 is connected to Start and ALE pin of ADC0808
sbit output_enable = P3^7;		 //P3^7 is connected to Output Enable Pin of ADC0808

unsigned char i=0;  //used in main function during ADC conversion 


void white_count_display(unsigned char number)
{
 unsigned char temp;
 temp=number/10;
 LCD_CMD(0x86);
 LCD_CHAR(temp+48);
 temp=number%10;
 LCD_CMD(0x87);
 LCD_CHAR(temp+48);
}

void black_count_display(unsigned char number)
{
 unsigned char temp;
 temp=number/10;
 LCD_CMD(0xC6);
 LCD_CHAR(temp+48);
 temp=number%10;
 LCD_CMD(0xC7);
 LCD_CHAR(temp+48);
}

void total_count_display(unsigned char number)
{
 unsigned char temp;
 temp=number/10;
 LCD_CMD(0xCB);
 LCD_CHAR(temp+48);
 temp=number%10;
 LCD_CMD(0xCC);
 LCD_CHAR(temp+48);
}


unsigned char ADC_conversion(unsigned char ADC_ch_no)
{
	unsigned char ADC_data;
	
	P0 = ADC_ch_no;		  // to select channel no. send address on P0
	
	start_conv = 1; // ADC0808 will latch the address on L-H transition on the ALE pin
	start_conv = 0;	// ADC0808 will start conversion on H-L transition on the start pin
	delay_ms(2);	// conversion time for ADC0808
	
	P0 =0xFF;	        // output enable and read
	output_enable = 1;	//Enabling o/p of ADC0808 for to read the converted data
	ADC_data = P0;		//storing adc_data
	output_enable = 0;	//disabling ADC 0808 o/p
	return ADC_data;	//returning adc_data
}


void servo_init()
{ /* Initial Timer0 Generate Overflow PCA */
	TMOD = 0x02; // 8-bit auto-reload Timer/Counter ‘THx’ holds a value which is to be reloaded into ‘TLx’ each time it overflows. 
	TH0  = 0xB8;     
	TR0=1;  	// To on/set Timer 0
    CMOD=0x04;	//Clock get from Timer 0, Timer0 overflow = 1 clock to PCA
    CCAPM0=0x42;
    CCAPM1=0x42;
    CCAPM2=0x42;
    CCAPM3=0x42;
    CCAPM4=0x42;
    CCAP0H=0x00;
    CCAP1H=0x00;
    CCAP2H=0x00;
    CCAP3H=0x00;
    CCAP4H=0x00;
    CCON=0x40;

    IEN0 = 0x00;
    P1 = 0xFF; /* Motor STOP */
    delay_ms(30);    
    IEN0 = 0x80; /* Start interrupt */
}

void run_servo(unsigned char servo_no, unsigned int angle)
{
  float value;
  value=(((angle*0.91666)/7.2)+(6.4));	// 25 Positions  || 6.4 is value for .5 mSec
										// 180 / 25 position = 7.2 Multiply factor
 								  		// Correction factor 0.91666
										// Correction at 165 -> 180 degree
  switch(servo_no)
  {
	case 1:	CCAP2H=(256-value); 
			break;					
	case 2:	CCAP3H=(256-value); 	
			break; 					
	case 3: CCAP4H=(256-value); 		
			break;
  }
}

void main(void)
{
 unsigned char white_count=0;	// White Ball Counter variable
 unsigned char black_count=0;	// Black Ball Counter variable
 unsigned char color_sensor_value=0; // Color Sensor
 LCD_INIT();
 servo_init();
 LCD_CMD(LINE1);
 LCD_WRITE("WHITE:00 | TOTAL");
 LCD_CMD(LINE2);
 LCD_WRITE("BLACK:00 | 00 ");
 LCD_CMD(DON_COFF);
 run_servo(3,90);
 color_sensor_value=ADC_conversion(3);
 while(1)
 {
   	 if(touch_sensor==0)//If Touch detected
	 {
		   delay_ms(2000);
		   color_sensor_value=ADC_conversion(3); //Read ADC value of Color Sensor fixed in 
		
		   if(color_sensor_value>30)
		   {
		   	run_servo(3,165);	//Drop Ball in Black Basket
			black_count++;		//Black Ball Counter Increment
			black_count_display(black_count);
			total_count_display(white_count+black_count);
			delay_ms(800);
		   }
		   else
		   {
		   	run_servo(3,15);	//Drop Ball in White Basket
			white_count++;		//White Ball Counter Increment
			white_count_display(white_count);
			total_count_display(white_count+black_count);
			delay_ms(800);
		   }
	 }
	 else
	 {
	  buzzer=1;	//Buzzer OFF
	  run_servo(3,90);
	 }
 } 
}
