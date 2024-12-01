#include <mega32.h>
#include <delay.h>

#define E PORTA.0
#define RS PORTA.1
#define E_delay 1


void send_command(char command){
  RS=0;
  PORTC=command;    
  E=0;
  delay_ms(E_delay); 
  E=1;
  RS=1; 
}

void lcd_init(void) {
  E=1;
  send_command(0x0E);
  send_command(0x38);
}

void clear_display(){
  send_command(0x01);
}

void display_shift(){
    send_command(0x18);
}

void char_out(char out){
   PORTC=out;
   E=0;
   delay_ms(E_delay);
   E=1;
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


void main(void)
{

  DDRC=0xFF;
  DDRA=0xFF;   
  //init LCD
  lcd_init();   
       
  menu(0);
  
                                                       

  while(1){
    display_shift();
    delay_ms(25);
  }    
  
}