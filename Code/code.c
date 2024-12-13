#include <mega32.h>
#include <delay.h>

#define E PORTC.4
#define RS PORTC.5
#define student_num 30             /////////  maximum can be 99

unsigned int time =0;
char page=0;
char cursor=0x00;
char code[9];                          /////////// one more to separate arrays
char student[student_num*8+1];
char show[6*13];



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


void display_shift(int x){
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
  print("",0x00); /////////////bug
 switch (page){     
  case 0:  
  print("1)Initialization 3)Presents    5)USART\n2)Search         4)Temperature 6)Traffic",0x00);         
  break;
  case 11:
  print("Attendance\nInitialization",0x00);
  break;         
  case 1:
  case 2:  
    print("Exit:* ,Delete:#\n",0x00);
    print(code,cursor);
  break;
  case 12:   
  case 22:
  print("Enter:*,Delete:#\n",0x00);  
  print(code,cursor);
  break;    
  case 21:
  print("Students\nManagment",0x00);    
  break;                               
  case 23:             
  print("Present",0x00);
  print(show,0x40);
  break;
  case 24:       
  print("Absent",0x00);    
  print(code,0x40);
  break;
  case 31: 
  print("Present Students\nExit:* Number:",0x00);      
  break;
  case 3:
  print(show,0x00);
  break;

 //default: menu(0);
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
    menu(0);
}


void set_list(char p){
    char i=p*6;    
    char t=0x00;
    for(;t<6*13;t++) show[t]=0x00;         
  for(;i<(p+1)*6;i++){
    char j=0;  
    char show_p= 13*(i-p*6);  
    if(student[8*i] == 0x00)  break;
    show[show_p]=(i+1)/10+0x30;
    show[show_p+1]=(i+1)%10+0x30;
    show[show_p+2]= ')';  
    show[show_p+11]= ' ';
    show[show_p+12]= ' ';    
                   
    for(;j<8;j++){   
       show[j+show_p+3]=student[j+8*i];       
    }                                           
  }  
  show[38]='\n';  
  show[77]=0x00;
}


unsigned int text_size(char * in){
  unsigned int out=0x00;  
  char *temp=in; 
  while(*temp++) {   
    out ++;
    }             
    return out;  
}

char check_student(){
  char*it=student;      
  char number=0 ;   
  char i=0; 
  show[11]=0x00;
  show[2]=')';
  
  while(*it){  
     i=0;
     for ( ;i<9;i++){
        if(*(it+i) != code[i]) break;
        if (code[i]==0x00){           
          show[0]=number/10;
          show[1]=number%10; 
          return 0x01;
        }  
        show[i+3]=code[i];
     }  
     number +=1;     
     it=student+number*8;
  }        
  return 0x00;    
}


void display(char go){ 

    ///////////////////////// timer

   if (page==0 && time%25==0 ){
    display_shift(time/25);  
    if(time==900) time=0;
  }                                       
  
  else if (page ==11 && time == 100 ){
    page=1;
    menu(1);
    time=0;
  }            
  
  else if((page == 21 || page == 23 || page == 24) && time == 200){
   page=2;
    menu(2);
    time=0; 
  }
  
  else if (page ==31 && time == 200){
    page =3;
    time=0;
    return;
  }   
    
    ///////// showing number in shift mode
   else if( page==3 && time%25==0 && go == 0x00){   
        char number=text_size(student)/8;   
        if (time==((number-1)/6+1)*900) time=0;  
        
       if (time%900 ==0) {      
          set_list(time/900 );
          menu(3);
        }          
        display_shift((time%900)/25);       
  }   
   ///////////////////////////////////////////////end timer
          
   ///////////////////////////////////////////click on page 0     
    if(page == 0 && go ==1){
    time =0;
    page=11;
    menu(11); 
  }   
  
   else if (page == 0 && go == 0x02){               
    time =0;
    page=21;
    menu(21);   
  }                  
  
   else if (page == 0 && go == 0x03){  
    char number=(text_size(student)/8)/10+0x30;   
    *((&number)+1)=(text_size(student)/8)%10+0x30;
    *((&number)+2)=0x00;                  
    time =0;
    page=31;
    menu(31);   
    print(&number,cursor);
  }     
    ////////////////////////////////// end click on page 0
  
  
  ///////////////////////////////////////////click on page 1  and 2                                                                                                       
     
  ////// press any key to skip info page    1
  else if(page == 11 && go != 0){
    page =1;
    menu(1);
  }     
  //// exit    1
  else if(page ==1 && go == 0xD1){                    
    char i=0x00; 
    for(;i<0x08;i++) code[i]=0x00;
   go_menu();     
  }    
  //// enter    1
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
   //// Delete     1
  else if((page ==1 || page == 12) && go == 0xD3){
   if (text_size(code))
     code[text_size(code)-0x01]=0x00;
      menu(1);
      page =1;
  }        
  
   ////// press any key to skip info page    2
  else if((page == 21 || page ==23 || page ==24) && go != 0){
    page =2;
    menu(2);
  }     
  //// exit     2
  else if(page ==2 && go == 0xD1){                    
    char i=0x00; 
    for(;i<0x08;i++) code[i]=0x00;
   go_menu();     
  }    
  //// enter    2
  else if (page==22 && go == 0xD1){    
    char i=0;  
    time=0;
    if(check_student()){
      page=23;
      menu(23);
    }  
    else{
      page=24;
      menu(24);   
  }               
  for(;i<8;i++) code[i]=0x00;
  }
   //// Delete   2
  else if((page ==2 || page == 22) && go == 0xD3){
   if (text_size(code))
     code[text_size(code)-0x01]=0x00;
      menu(2);
      page =2;
  } 
  
   
  /// input numbers
  else if(((page ==1 || page==12) || (page ==2 || page==22)) && go!=0xD1 && go!= 0xD3 && go != 0x00){   
    /// buzzer error
    if (text_size(code) == 0x08){
      print("Error",cursor+2);    
      PORTC.6=1;
      delay_ms(25);
      PORTC.6=0;
      return ;
    }        
    //number
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
    
      /// go to enter mode 
     if (text_size(code) == 0x08){ 
      if (page ==1){
      menu(12);
      page=12;  
      }
      else{ 
       menu(22);
      page=22;
      } 
     } 
     }  
          
    
  ////////////////////////////////// end click on page 1    and   2
                                                                                                     

  
                                                        
  /////////////////////////////// click on page 3
  
  else if(page ==31 && go!= 0x00){
     page =3;
    time=0;
    return;
  }                                                                                 
  
  else if ( page ==3 && go== 0xD1){ 
    go_menu();
  }                                                                   
  //////////////////////////end click page 3

  time=time+1; 
}

void main(void)
{                                                    
  DDRC=0xFF;    
  DDRA=0xF8;
  //DDRD=0xFF;  
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