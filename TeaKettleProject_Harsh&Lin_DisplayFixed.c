/*
 * Author: Harsh Dubey
 * Lab partner: Lin Zeng
 * Code: Tea kettle Project Updated
 * Created on Feb 2, 2018
*/
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h> 
#include <xc.h>
#include "newfile.h"

//Configurating bits
#define _XTAL_FREQ  8000000
#pragma config PBADEN = OFF//PORTB as digital Inputs
#pragma config FOSC = HSMP
#pragma config FCMEN = OFF
#pragma config WDTEN = OFF

//Change Inputs here
#define column_1 PORTBbits.RB2

#define column_2 PORTBbits.RB3
#define column_3 PORTBbits.RB0
#define column_4 PORTBbits.RB1
#define ready PORTDbits.RD7
#define ON PORTAbits.RA1


//change output here
#define row_out PORTD

//Prototype 
void POLLING(signed char key_count[2],signed char temp[3],signed char value[2],unsigned char enter[1],int compose[3]);
//Don't know why this is needed
void Oscillator_Initialize()
{
    OSCCON = 0x30;
    OSCCON2 = 0x04;
    OSCTUNE = 0x00;
}

void main()
{
    //ROW direction,OUTPUT
    TRISDbits.RD3 = 0;//Why single initializing??
    TRISDbits.RD2 = 0;//Well, portd 4,5,6 is used by LCD//Don't mess with them
    TRISDbits.RD1 = 0;
    TRISDbits.RD0 = 0;
    TRISEbits.RE0 = 0;
    TRISEbits.RE1 = 0;
    TRISEbits.RE2 = 0;
    //TRISDbits.RD7 = 0;
    TRISAbits.RA1 = 1;
    ANSA1=0;
    //Column Direction,INPUT
    TRISBbits.RB0 = 1;//Why?? easy to make em digital input
    TRISBbits.RB1 = 1;//Can't use RB6,RB5,RB7
    TRISBbits.RB2 = 1;
    TRISBbits.RB3 = 1;
    TRISAbits.RA0 = 1;//ADC selecting RA0 as input
    TRISDbits.RD7 = 0;
    //Needed for LCD
    LATC = 0x00;
    TRISC = 0xD7;
    ANSELC = 0xC4;//PORTC is used by the LCD//Do not use it anywhere else
    SPI1_Initialize();
    Oscillator_Initialize();
    lcdinit();
    //All six Lines of Codes
    //ADC CONTROL Bits setting
	ANSA0=1;  // select RA0 as analog input 
    ADCON2 =0xAD;  // left justified, 12TAC
    ADCON1 =0x00;  //reference voltage vcc,vss
	ADCON0 =0x01;  //RA0 as analog input to ADC
    int TheH,TheL;
    unsigned long int tempval; //Reserve For ADC number conversion
    
    int current;//thermister data reading
    //ADC ends
    //keypad Variables
    signed char key_count[2];
    signed char temp[3];
    signed char value[2];
    unsigned char enter[1];
    
    int compose[3];
    int desired[4];
    //KeyPad Ends
    
    PORTE = 0x0;
    ready = 0;
    if(ON)
    {
        lcdCommand(0x01);
        temp[1]=-16;
        temp[2]=-16;
    //STATE1: Start             //Ask the user for Temp//
       if(enter[1]==0)
       {  
           while(enter[1]==0&&ON)
       {
               PORTE = 0x0;
               ready = 0;
 
       lcdChar('E');
       lcdChar('N');
       lcdChar('T');
       lcdChar('E');
       lcdChar('R');
       lcdChar(' ');
       lcdChar('T');
       lcdChar('E');
       lcdChar('M');
       lcdChar('P');
       lcdChar(' ');
       lcdChar('I');
       lcdChar('N');
       lcdChar(' ');
       lcdChar('C');
       lcdGoTo(0x40);{
       lcdChar(' ');
       lcdChar(' ');
       lcdChar(' ');
       lcdChar(' ');
       lcdChar(' ');
       lcdChar(' ');
       lcdChar(' ');
       lcdChar(temp[1]+48);
       lcdChar(temp[2]+48);
       lcdChar(' ');
       lcdChar(' ');
       lcdChar(' ');
       lcdChar(' ');
       lcdChar(' ');
       lcdChar(' ');
       lcdChar(' ');              }
       row_out = 0X00;
    if(!column_1||!column_2||!column_3)      //Poll for key press
    {
    __delay_ms(8);
    if(!column_1||!column_2||!column_3) 
    {key_count[1]++;}
    }POLLING(key_count,temp,value,enter,compose);                        //Only accept when button hit
       }
       }//Exit at Enter press
    //STATE1:ENDS
 
    //STATE2: STARTS
    if(enter[1] == 1&&ON)//Once value is entered
    {
        lcdCommand(0x01);
    
    while(enter[1]==1&&ON)      //Stay in this loop forever un till a keypress
    {
        
    ADCON0bits.GO = 1;
    current=ADRES;
    tempval=current;
    if(compose[1]<69)   //Different slope at temps less than 69
     {
        current=((25.36715*(tempval*0.00488743+0.001820555))-23.20360481);
     }
     else
     {
        current=((25.36715*(tempval*0.00488743+0.001820555))-26.20360481);
     } 
    
	 TheL=current%10;          //Decomposing in Decimal //val3=val2%10;//Since, C does a better job
	 TheH=current/10;
     ADCON0bits.GO = 0;
     
    if(current<(compose[1]))//Check and start heating
    { 
    
        ready=0;
        PORTE = 0x3;
    
    lcdCommand(0x01);
    lcdGoTo(0x40);
    {  lcdChar(' ');
       lcdChar(' ');
       lcdChar('D');
       lcdChar('E');
       lcdChar('S');
       lcdChar('I');
       lcdChar('R');
       lcdChar('E');
       lcdChar('D');
       lcdChar(' ');
       lcdChar(':');
       lcdChar(' ');
       lcdChar(value[1]+48);
       lcdChar(value[2]+48);
    }
    for(int i =0; i<250; i++)     //Delay + Poll
    {
        
    row_out = 0X00;
    if(!column_1||!column_2||!column_3)
    {
    enter[1]=0;
    i=511;
    }
     ADCON0bits.GO = 1;//For faster updates
     current=ADRES;
     tempval=current; 
     if(compose[1]<51)   
     {
         current=((25.36715*(tempval*0.00488743+0.001820555))-23.20360481);
     }
     else
     {
         current=((25.36715*(tempval*0.00488743+0.001820555))-26.20360481);
     }
	 TheL=current%10;          //Decomposing in Decimal //val3=val2%10;//Since, C does a better job
	 TheH=current/10;
     ADCON0bits.GO = 0;
    __delay_ms(5);
    }
    
    PORTE = 0x2;
    lcdCommand(0x01);
    lcdGoTo(0x40);
    {  
       lcdChar(' ');
       lcdChar(' ');
       lcdChar('C');
       lcdChar('U');
       lcdChar('R');
       lcdChar('R');
       lcdChar('E');
       lcdChar('N');
       lcdChar('T');
       lcdChar(' ');
       lcdChar(':'); 
       lcdChar(' ');
	   lcdChar(TheH+48);
	   lcdChar(TheL+48);		
    }
    for(int i =0; i<260; i++)      //Delay + Poll
    {
    row_out = 0X00;
    if(!column_1||!column_2||!column_3)
    {
    enter[1]=0;
    i=511;
    }
    __delay_ms(5);
    }
    
    }else if(current>compose[1]+2)//IF temp exceeds 2 degree celcius
    {ready = 0;
    for(int i =0; i<15; i++)
    {
       PORTE = 0X4;
       lcdCommand(0x01);//Warning
       lcdGoTo(0x40);
       lcdChar(' ');
       lcdChar(' ');
       lcdChar(' ');
       lcdChar(' ');
       lcdChar('W');
       lcdChar('A');
       lcdChar('R');
       lcdChar('N');
       lcdChar('I');
       lcdChar('N');
       lcdChar('G');
       lcdChar('!');
       lcdChar(' ');
       lcdChar(' ');
       lcdChar(' ');
       lcdChar(' ');
       for(int i =0; i<51; i++)      //Delay + Poll
       {
       __delay_ms(1);
       }
       PORTE = 0X0;
       for(int i =0; i<51; i++)      //Delay + Poll
       {
       __delay_ms(1);
       }
    }
    enter[1]=2;
    
    }else if((current+1)<=compose[1]<=(current))//Once the temperature is reached
    {//ready = 1;
       PORTE = 0X0;
       ready = 1;
       lcdGoTo(0x40);
       lcdChar(' ');
       lcdChar(' ');
       lcdChar('T');
       lcdChar('E');
       lcdChar('A');
       lcdChar(' ');
       lcdChar('I');
       lcdChar('S');
       lcdChar(' ');
       lcdChar('R');
       lcdChar('E');
       lcdChar('A');
       lcdChar('D');
       lcdChar('Y');
       lcdChar(' ');
       lcdChar(' ');
       
       for(int i =0; i<500; i++)      //Delay + Poll
    {
    row_out = 0X00;
    if(!column_1||!column_2||!column_3)
    {
    enter[1]=0;
    i = 510;
    }
    }  
    }
    }
    }
        
    //STATE 3
    if(enter[1]==2&&ON)//Warning+actual temp
    {
        lcdCommand(0x01);
    while(enter[1]==2&&ON)
    {
    if(current<compose[1]+2)
    {
        enter[1]=1;
    }else{
    ready=0;
    ADCON0bits.GO = 1;
    current=ADRES;
    tempval=current;
        
     if(compose[1]<51)   
     {
         current=((25.36715*(tempval*0.00488743+0.001820555))-24.20360481);
     }
     else
     {
         current=((25.36715*(tempval*0.00488743+0.001820555))-26.20360481);
     }
    
	 TheL=current%10;          //Decomposing in Decimal //val3=val2%10;//Since, C does a better job
	 TheH=current/10;
     ADCON0bits.GO = 0;
     
        PORTE = 0x4;
        lcdGoTo(0x40);
        lcdChar('A');
        lcdChar('C');
        lcdChar('T');
        lcdChar('U');
        lcdChar('A');
        lcdChar('L');
        lcdChar(' ');
        lcdChar('T');
        lcdChar('E');
        lcdChar('M');
        lcdChar('P');
        lcdChar(' ');
        lcdChar(':');
        lcdChar(' ');
        lcdChar(TheH+48);
	    lcdChar(TheL+48);
        
    row_out = 0X00;
    if(!column_1||!column_2||!column_3)
    {
    temp[1]=-16;
    temp[2]=-16;
    enter[1]=0;
    }
    }
    }
    }
    }
    else if(!ON)//Clear LCD if 
    {
       lcdCommand(0x01);
       enter[1]=0;
       temp[1]=-16;
       temp[2]=-16;
    }
}
    //While

void POLLING(signed char key_count[2],signed char temp[3],signed char value[2],unsigned char enter[1],int compose[3])
{
    //Compose and increment
    
    //VALUE SCANNER
    if(key_count[1]==1)
    {
        //Checking for updates
        row_out = 0X0E;//ROW1
        //Pin 1
        if(!column_1)
        {
            temp[1]=1;
            temp[2]=-16;
        }
        //Pin 2
        if(!column_2)
        {
            temp[1]=2;
            temp[2]=-16;
        }
        //Pin 3
        if(!column_3)
        {
            temp[1]=3;
            temp[2]=-16;
        }
        //Ends
        //ROW2
        row_out = 0X0D;
        if(!column_1)
        {
            temp[1]=4;
            temp[2]=-16;
        }
        //Pin 2
        if(!column_2)
        {temp[1]=5;
        temp[2]=-16;}
        //Pin 3
        if(!column_3)
        {
            temp[1]=6;
            temp[2]=-16;
        }
        //Ends
        //ROW3
        row_out = 0X0B;
        if(!column_1)
        {
            temp[1]=7;
            temp[2]=-16;
        }
        //Pin 2
        if(!column_2)
        {
            temp[1]=8;
            temp[2]=-16;
        }
        //Pin 3
        if(!column_3)
        {
            temp[1]=9;
            temp[2]=-16;
        }
        //Ends
        //ROW4
        row_out = 0X07;
        if(!column_1)
        {
            temp[1]=0;
            temp[2]=-16;
        }
        //Pin 2
        if(!column_2)
        {
            temp[1]=0;
            temp[2]=-16;
        }
        //Pin 3
        if(!column_3)
        {
            temp[1]=0;
            temp[2]=-16;
        }
        //Temp[1] update ends
    }
    //Poll for second digit if first digit is entered
     if(key_count[1]==2)
    {
         row_out = 0X0E;//ROW1
         //PIN 1
        if(!column_1)
        {
            temp[2]=1;
            key_count[1]=0;
        }
        //PIN 2
        if(!column_2)
        {
            temp[2]=2;
            key_count[1]=0;
        }
        //PIN3
        if(!column_3)
        {
            temp[2]=3;
            key_count[1]=0;
        }
        //Ends
        //ROW2
        row_out = 0X0D;
        if(!column_1)
        {
            temp[2]=4;
            key_count[1]=0;
        }
        //PIN 2
        if(!column_2)
        {
            temp[2]=5;
            key_count[1]=0;
        }
        //PIN3
        if(!column_3)
        {
            temp[2]=6;
            key_count[1]=0;
        }
        //Ends
        //ROW3
        row_out = 0X0B;
        if(!column_1)
        {
            temp[2]=7;
            key_count[1]=0;
        }
        //PIN 2
        if(!column_2)
        {
            temp[2]=8;
            key_count[1]=0;
        }
        //PIN3
        if(!column_3)
        {
            temp[2]=9;
            key_count[1]=0;
        }
        //Ends
        //ROW 4
        row_out = 0X07;
        if(!column_1)
        {
            temp[2]=0;
            key_count[1]=0;
        }
        //PIN 2
        if(!column_2)
        {
            temp[2]=0;
            key_count[1]=0;
        }
        //PIN3
        if(!column_3)
        {
            temp[2]=0;
            key_count[1]=0;
        }
        //Ends
    }
    
    //CLEAR BUTTON
    row_out = 0X0E;
    //PIN4
        if(!column_4)
        {
            temp[1]=-16;
            temp[2]=-16;
            key_count[1]=0;
            value[1]=-16;
            value[2]=-16;
            enter[1]=0;
        }
    //Clear Ends
    
    //Increment
    row_out = 0X0D;
    if(!column_4)
    {   compose[1]= ((temp[1]*10)+temp[2]);
        if(compose[1]<98)
        {
        if(temp[2]!=-16&&temp[1]!=-16)
        {
            compose[1]++;
            temp[1]=compose[1]/10;
            temp[2]=compose[1]%10;
        }  
        else if(temp[2]==-16&&temp[1]==-16)
        {
            compose[1]=0;
            compose[1]++;
            temp[1]=compose[1]/10;
            temp[2]=compose[1]%10;
        }  
        else if(temp[2]==-16&&temp[1]!=-16)
        {
            temp[1]++;
            if(temp[1]>9)
            {
                temp[2]=0;
             temp[1]=1;
            }            
        }  
      }
    }
    //Increment Ends
    //Decrement
    row_out = 0X0B;
    if(!column_4)
    {
        compose[1]= ((temp[1]*10)+temp[2]);
        if(compose[1]<100&&compose[1]>0)
        {
        if(temp[2]!=-16&&temp[1]!=-16)
        {
            compose[1]--;
            temp[1]=compose[1]/10;
            temp[2]=compose[1]%10;
        }  
        else if(temp[2]==-16&&temp[1]==-16)
        {
            Nop();
        }  
        else if(temp[2]==-16&&temp[1]!=-16)
        {
            temp[1]--;   
            
        }
        
        }
    }
    //Decrement Ends
    //ENTER//OUTSIDE SINCE ANY MOMENT IN TIME
    row_out = 0X07;
    if(!column_4)
    {
    compose[1]= ((temp[1]*10)+temp[2]);
    if(compose[1]<99&&compose[1]>=25)
    {
        if((temp[1]!=-16&temp[2]!=-16)&&(temp[1]!=0&&temp[2]!=0))//[x,x]&&[!0,!0]
        {
        value[1] = temp[1];
        value[2] = temp[2];
        temp[1]=-16;
        temp[2]=-16;
        key_count[1]=0;
        enter[1]=1;
        }
    else if((temp[1]!=-16&&temp[2]==0)&&temp[1]!=0)//[x,0]&&[x!=0]
        {
        value[1] = temp[1];
        value[2] = temp[2];
        temp[1]=-16;
        temp[2]=-16;
        key_count[1]=0;
        enter[1]=1;
        }
    else if((temp[1]==0&&temp[2]!=-16)&&(temp[2]!=0))//[0,x]&&[x!=0]
        {
        value[1] = temp[1];
        value[2] = temp[2];
        temp[1]=-16;
        temp[2]=-16;
        key_count[1]=0;
        enter[1]=1;
        }
    else if((temp[1]!=-16&&temp[2]==-16)&&(temp[1]!=0))//[x,0]&&[x!=0]
        {
           __delay_ms(5);
           if(!column_4)
           {
            while(!column_4)
            {
            lcdGoTo(0x40);
            lcdChar(' ');
            lcdChar(' ');
            lcdChar('W');
            lcdChar('R');
            lcdChar('O');
            lcdChar('N');
            lcdChar('G');
            lcdChar(' ');
            lcdChar('T');
            lcdChar('E');
            lcdChar('M');
            lcdChar('P');
            lcdChar('!');
            lcdChar(' ');
            lcdChar(' ');
            lcdChar(' ');
            }
           }
        }
    else if(temp[1]==0&&temp[2]==0)//[0,0]
        {
           __delay_ms(5);
           if(!column_4)
           {
            while(!column_4)
            {
            lcdGoTo(0x40);
            lcdChar(' ');
            lcdChar(' ');
            lcdChar('W');
            lcdChar('R');
            lcdChar('O');
            lcdChar('N');
            lcdChar('G');
            lcdChar(' ');
            lcdChar('T');
            lcdChar('E');
            lcdChar('M');
            lcdChar('P');
            lcdChar('!');
            lcdChar(' ');
            lcdChar(' ');
            lcdChar(' ');
            }
           }
        }
    else if(temp[1]==0&&temp[2]==-16)//[0,-16]
        {
           __delay_ms(5);
           if(!column_4)
           {
        while(!column_4)
        {
            lcdGoTo(0x40);
            lcdChar(' ');
            lcdChar(' ');
            lcdChar('W');
            lcdChar('R');
            lcdChar('O');
            lcdChar('N');
            lcdChar('G');
            lcdChar(' ');
            lcdChar('T');
            lcdChar('E');
            lcdChar('M');
            lcdChar('P');
            lcdChar('!');
            lcdChar(' ');
            lcdChar(' ');
            lcdChar(' ');
        }
           }
        }
    else if(temp[1]==-16&&temp[2]==-16)//[-16,-16]
        {
           __delay_ms(5);
           if(!column_4)
           {
        while(!column_4)
        {
            lcdGoTo(0x40);
            lcdChar(' ');
            lcdChar(' ');
            lcdChar('W');
            lcdChar('R');
            lcdChar('O');
            lcdChar('N');
            lcdChar('G');
            lcdChar(' ');
            lcdChar('T');
            lcdChar('E');
            lcdChar('M');
            lcdChar('P');
            lcdChar('!');
            lcdChar(' ');
            lcdChar(' ');
            lcdChar(' ');
        }
           }
    }
        }
    if(compose[1]>98||compose[1]<25)
    {
        __delay_ms(5);
        if(!column_4)
        {
        while(!column_4)
        {
            lcdGoTo(0x40);
            lcdChar(' ');
            lcdChar(' ');
            lcdChar('W');
            lcdChar('R');
            lcdChar('O');
            lcdChar('N');
            lcdChar('G');
            lcdChar(' ');
            lcdChar('T');
            lcdChar('E');
            lcdChar('M');
            lcdChar('P');
            lcdChar('!');
            lcdChar(' ');
            lcdChar(' ');
            lcdChar(' ');
        }
        }
    }
    }
       return;
    }
  


