#include <mega32.h>
#include <delay.h>

#define E PORTC.4
#define RS PORTC.5


void send_lcd( char bits){
  PORTC=PORTC & 0xF0 | bits;  
  E=0;
  delay_ms(1); 
  E=1;
}

void send_command(char command){
  RS=0;     
  send_lcd (( command & 0xF0 ) >> 4);
  send_lcd( command & 0x0F );                    
  RS=1; 
}

void char_out(char out){
  send_lcd(( out & 0xF0 ) >> 4);
  send_lcd( out & 0x0F );
}

void lcd_init(void) {
  E=1;     
  send_command(0x33);
  send_command(0x32);
  send_command(0x28);
  send_command(0x0E);
}

void clear_display(){
  send_command(0x01);
}

void display_shift(){
    send_command(0x18);
}

void iter(unsigned char it){
  send_command(it);
}

void print(char* out, unsigned char it){
  it=it+0x80;  
  iter(it);
  while(*out){       
  if(*out==0x0A){  
      it =it+0x40; 
      iter(it);        
      *out++;
  }
  else{
      char_out(*out++);
    }
  }
}

void menu(char page){
clear_display();
 switch (page){
  case 0: 
  print("1)Initialization 3)Presents    5)USART\n2)Search         4)Temperature 6)Traffic",0x00); 
  break;

 default: menu(0);
 };
}


char row(char col){
      char out;
      PORTA.3=1;
      if(( PINA & 0x07) != col) out = 0xA0;      
      PORTA.3=0;
      PORTA.4=1;
      if(( PINA & 0x07) != col) out = 0xB0;
      PORTA.4=0;
      PORTA.5=1;
      if(( PINA & 0x07) != col) out = 0xC0;
      PORTA.5=0;
      PORTA.6=1;
      if(( PINA & 0x07) != col) out = 0xD0;
      PORTA.6=0;    
      return out;
}

char key(){
  if ( (PINA & 0x07) == 0x07  ) return  0x00 ; 
  if ((PINA & 0x07) == 0x03  )   return 0x01 | row(0x03);
  if ((PINA & 0x07) == 0x05  )   return 0x02 | row(0x05 );
  if ((PINA & 0x07) == 0x06  )   return 0x03 | row(0x06);
}


void main(void)
{

  DDRC=0xFF;    
  DDRA=0xF8;
  //init LCD
  lcd_init();              
  //init key pad
  PORTA=0x07;
     
  menu(0);
                                           

  while(1){
   display_shift();
   delay_ms(25);   
   key();
  }    
  
}