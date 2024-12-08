#include <mega32.h>
#include <delay.h>

#define E PORTC.4
#define RS PORTC.5
#define student_num 30             /////////  maximum can be 99

int time =0;
char page=0;
char x=0;
char cursor=0x00;
char code[9];                          /////////// one more to separate arrays
char student[student_num*8];



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

void iter(unsigned char it){
  send_command(it);
}


void display_shift(){
x+=1;
  if(x>6 && x<=30){  
   send_command(0x18); 
    }  
  if (x == 36){
    x=0;     
    send_command(0x02);
    }   
}

void print(char* out, unsigned char it){
  it=it+0x80;
  cursor=it-0x80;  
  iter(it);
  while(*out){       
  if(*out==0x0A){  
      it =it+0x40; 
      iter(it);        
      *out++;
      cursor=it-0x80;
  }
  else{
      char_out(*out++);
      cursor++;
    }
  }
}

void menu(char page){
clear_display();
 switch (page){
  case 0: 
  print("1)Initialization 3)Presents    5)USART\n2)Search         4)Temperature 6)Traffic",0x00); 
  break;
  case 11:
  print("Attendance\nInitialization",0x00);
  break;         
  case 1:  
    print("Exit:* ,Delete:#\n",0x00);
    print(code,cursor);
  break;
  case 12: 
  print("Enter:*,Delete:#\n",0x00);  
  print(code,cursor);
  break;
  case 31: 
  print("Present Students\nExit:* Number:",0x00);      
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

char key_press(){
  if ((PINA & 0x07) == 0x07  ) return  0x00 ; 
  if ((PINA & 0x07) == 0x03  )   return 0x01 | row(0x03);
  if ((PINA & 0x07) == 0x05  )   return 0x02 | row(0x05 );
  if ((PINA & 0x07) == 0x06  )   return 0x03 | row(0x06);
}

char pressed =0;
char key(){
  if( pressed ==0) 
  {      
    pressed=key_press();
    if (pressed>0x00 &&  pressed < 0xD1){    
        pressed=((pressed>>4)-0b1010)*3 + (pressed & 0x0F);   
       return pressed;
    }     
    return pressed;       
   } 
   pressed=key_press(); 
   return 0;  
}



void go_menu(){
    time =0;
    page =0;
    x=0;
    menu(0);
}


unsigned int text_size(char * in){
  unsigned int out=0x00;  
  char *temp=in; 
  while(*temp++) {   
    out ++;
    }             
    return out;  
}


void display(char go){
  time=time+1;  
   if (page==0 && time==25 ){
    display_shift();
    time=0;
  }       
  else if (page ==11 && time == 100 ){
    page=1;
    menu(1);
    time=0;
  }
  else if (page ==31 && time == 200){
    page =3;
    print("all right",0x00);
    time=0;
  }
  
  
  if(page == 0 && go ==1){
    time =0;
    page=11;
    menu(11); 
  }
  else if(page == 11 && go != 0){
    page =1;
    menu(1);
  } 
  else if(page ==1 && go == 0xD1){                    
    char i=0x00; 
    for(;i<0x08;i++) code[i]=0x00;
   go_menu();     
  }    
  
  else if (page==12 && go == 0xD1){ 
    unsigned int temp=text_size(student);   
    char i=0x00; 
    for(;i<0x08;i++){
      student[i+temp]=code[i];
     code[i]=0x00;
     } 
    menu(1);
    page=1;
  }
  
  else if((page ==1 || page == 12) && go == 0xD3){
   if (text_size(code))
     code[text_size(code)-0x01]=0x00;
      menu(1);
      page =1;
  } 
   
  else if((page ==1 || page==12) && go!=0xD1 && go!= 0xD3 && go != 0x00){   
    if (text_size(code) == 0x08){
      print("Error",cursor+2);    
      PORTC.6=1;
      delay_ms(25);
      PORTC.6=0;
      return ;
    }   
    if(go == 0xD2){  
     print("0",cursor);
     code[text_size(code)]=0x30;  
     }  
     
    else{
       char num=go+0x30;     
       char *p=&num;      
       *(p+1)=0x00;
       print(p,cursor);  
       code[text_size(code)]=num;  
     }    
     if (text_size(code) == 0x08){
      menu(12);
      page=12;    
     }
  }
    
  

  else if (page == 0 && go == 0x03){  
    char number=text_size(student)/8+0x30;     ///////////bug  2 number 
    *((&number)+1)=0x00;                 
    PORTD=number;
    time =0;
    page=31;
    menu(31);   
    print(&number,cursor);
  } 
   
  else if ((page == 31 || page ==3) && go== 0xD1){
    go_menu();
  }
   
}

void main(void)
{                                                    
  DDRC=0xFF;    
  DDRA=0xF8;  
  DDRD=0xFF; 
  //init LCD
  lcd_init();              
  //init key pad
  PORTA=0x07;   
  
     
  menu(0);
                                           

  while(1){     
      delay_ms(1);                    
      display(key());
  }    
  
}