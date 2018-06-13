#include<reg51.h>
#include<string.h>
#include<math.h>
#include<stdio.h>
#include<stdlib.h>

 
//Define Macros
#define Error  13    // Any value other than 0 to 9 is good here
 
//Function declarations
void cct_init(void);
void delay(int);
void lcdinit(void);
void writecmd(int);
void writedata(char);
void writeline(char[]);
void ReturnHome(void);
char READ_SWITCHES(void);
char get_key(void);
int get_num(char);
char get_func(char);
void DispError(int);
void disp_num(int);
void dval(double);
void ftoa(float n, char *res, int afterpoint);

 
//*******************
//Pin description
/*
P2 is data bus
P3.7 is RS
P3.6 is E
P1.0 to P1.3 are keypad row outputs
P1.4 to P1.7 are keypad column inputs
*/
//********************
// Define Pins
//********************
sbit RowA = P1^0;     //RowA
sbit RowB = P1^1;     //RowB
sbit RowC = P1^2;     //RowC
sbit RowD = P1^3;     //RowD
 
sbit C1   = P1^4;     //Column1
sbit C2   = P1^5;     //Column2
sbit C3   = P1^6;     //Column3
sbit C4   = P1^7;     //Column4
 
sbit E    = P3^6;     //E pin for LCD
sbit RS   = P3^7;     //RS pin for LCD
 
 char res[20];
 float result;
// ***********************************************************
// Main program
//
int main(void)
{
   char key; 
		char op;
   int num1 = 0;                 //First number
   char func = '+';              //Function to be performed among two numbers
   int num2 = 0;
float n1,n2;	//Second number
   char a[20];
	
	int i=0,j,k;
   cct_init();                   //Make input and output pins as required
   lcdinit();                    //Initilize LCD
	
	while(1){
		key=get_key();
		
		if(key!='=' && key!='C'){
			
			a[i]=key;
			writedata(a[i]);
			i++;
		}
		else if(key=='C'){
			writecmd(0x01);
			ReturnHome();
		}
		else if(key=='='){
			writedata(key);
			delay(1000);
			//writecmd(0x01);
			//ReturnHome();
			writecmd(0xC0);
			break;
		}
		
	}
	for(j=0;j<=i;j++){
		if(a[j]=='+' || a[j]=='-' || a[j]=='x' || a[j]=='/')
		{
			//writedata(a[j]);
			for(k=0;k<j;k++){
				int n=get_num(a[k]);
				num1=n+(num1*10);
			}
			
			op=a[j];
			for(k=(j+1);k<i;k++){
				int n=get_num(a[k]);
				num2=n+(num2*10);
			}
			//disp_num(num2);
			break;
		}
	}
	/*for(j=0;j<i;j++){
			writedata(a[j]);
			delay(1500);
		}*/
n1=(float)num1;
	n2=(float)num2;
	delay(35000);
	
	switch(op){
		case '+': result=(float)(num1+num2); break;
					 case '-': result=(float)(num1-num2); break;
					 case 'x': result=(float)(num1*num2); break;
					 case '/': result=n1/n2; break;
	}
	ftoa(result, res, 4);
	if(get_key()=='C'){ReturnHome();}
	
	
}
 

void cct_init(void)
{
	P0 = 0x00;   //not used
	P1 = 0xf0;   //used for generating outputs and taking inputs from Keypad
	P2 = 0x00;   //used as data port for LCD
	P3 = 0x00;   //used for RS and E   
}
 
void delay(int a)
{
   int i;
   for(i=0;i<a;i++);   //null statement
}
 
void writedata(char t)
{
   RS = 1;             // This is data
   P2 = t;             //Data transfer
   E  = 1;             // => E = 1
   delay(150);
   E  = 0;             // => E = 0
   delay(150);
}
 
 
void writecmd(int z)
{
   RS = 0;             // This is command
   P2 = z;             //Data transfer
   E  = 1;             // => E = 1
   delay(150);
   E  = 0;             // => E = 0
   delay(150);
}
 
void lcdinit(void)
{
  ///////////// Reset process from datasheet /////////
     delay(15000);
   writecmd(0x30);
     delay(4500);
   writecmd(0x30);
     delay(300);
   writecmd(0x30);
     delay(650);
  /////////////////////////////////////////////////////
   writecmd(0x38);    //function set
   writecmd(0x0c);    //display on,cursor off,blink off
   writecmd(0x01);    //clear display
   writecmd(0x06);    //entry mode, set increment
}
 
void ReturnHome(void)     /* Return to 0 cursor location */
{
   writecmd(0x02);
   delay(1500);
   ////
}
 
void writeline(char Line[])
{
   int i;
   for(i=0;i<strlen(Line);i++)
   {
      writedata(Line[i]);     /* Write Character */
   }
   
   ReturnHome();          /* Return to 0 cursor position */
}
 
char READ_SWITCHES(void)	
{	
	RowA = 0; RowB = 1; RowC = 1; RowD = 1; 	//Test Row A
 
	if (C1 == 0) { delay(10000); while (C1==0); return '7'; }
	if (C2 == 0) { delay(10000); while (C2==0); return '8'; }
	if (C3 == 0) { delay(10000); while (C3==0); return '9'; }
	if (C4 == 0) { delay(10000); while (C4==0); return '/'; }
 
	RowA = 1; RowB = 0; RowC = 1; RowD = 1; 	//Test Row B
 
	if (C1 == 0) { delay(10000); while (C1==0); return '4'; }
	if (C2 == 0) { delay(10000); while (C2==0); return '5'; }
	if (C3 == 0) { delay(10000); while (C3==0); return '6'; }
	if (C4 == 0) { delay(10000); while (C4==0); return 'x'; }
	
	RowA = 1; RowB = 1; RowC = 0; RowD = 1; 	//Test Row C
 
	if (C1 == 0) { delay(10000); while (C1==0); return '1'; }
	if (C2 == 0) { delay(10000); while (C2==0); return '2'; }
	if (C3 == 0) { delay(10000); while (C3==0); return '3'; }
	if (C4 == 0) { delay(10000); while (C4==0); return '-'; }
	
	RowA = 1; RowB = 1; RowC = 1; RowD = 0; 	//Test Row D
 
	if (C1 == 0) { delay(10000); while (C1==0); return 'C'; }
	if (C2 == 0) { delay(10000); while (C2==0); return '0'; }
	if (C3 == 0) { delay(10000); while (C3==0); return '='; }
	if (C4 == 0) { delay(10000); while (C4==0); return '+'; }
 
	return 'n';           	// Means no key has been pressed
}
 
char get_key(void)           //get key from user
{
	char key = 'n';              //assume no key pressed
 
	while(key=='n')              //wait untill a key is pressed
		key = READ_SWITCHES();   //scan the keys again and again
 
	return key;                  //when key pressed then return its value
}
 
int get_num(char ch)         //convert char into int
{
	switch(ch)
	{
		case '0': return 0; break;
		case '1': return 1; break;
		case '2': return 2; break;
		case '3': return 3; break;
		case '4': return 4; break;
		case '5': return 5; break;
		case '6': return 6; break;
		case '7': return 7; break;
		case '8': return 8; break;
		case '9': return 9; break;
		case 'C': writecmd(0x01); return Error; break;  //this is used as a clear screen and then reset by setting error
		default: DispError(0); return Error; break;     //it means wrong input
	}
}
 
char get_func(char chf)            //detects the errors in inputted function
{
	if(chf=='C')                   //if clear screen then clear the LCD and reset
	{ 
		writecmd(0x01);            //clear display
		////
		return 'e'; 
	}
	
	if( chf!='+' && chf!='-' && chf!='x' && chf!='/' )  //if input is not from allowed funtions then show error
	{ 
		DispError(1); 
		////
		return 'e'; 
	}
 
	return chf;                        //function is correct so return the correct function
}
 
 
void DispError(int numb)           //displays differet error messages
{
	writecmd(0x01);                //clear display
	////
	switch(numb)
	{
	case 0: 	writeline("Wrong Input");      break;
	case 1: 	writeline("Wrong Function");   break;
	default:    writeline("Wrong Input");      break;
	}
}
 
void disp_num(int numb)            //displays number on LCD
{	
	unsigned char UnitDigit  = 0;  //It will contain unit digit of numb
	unsigned char TenthDigit = 0;  //It will contain 10th position digit of numb
 
	if(numb<0)
	{
		numb = -1*numb;  // Make number positive
		writedata('-');	 // Display a negative sign on LCD
	}
 
	TenthDigit = (numb/10);	          // Findout Tenth Digit
 
	if( TenthDigit != 0)	          // If it is zero, then don't display
		writedata(TenthDigit+0x30);	  // Make Char of TenthDigit and then display it on LCD
 
	UnitDigit = numb - TenthDigit*10;
 
	writedata(UnitDigit+0x30);	  // Make Char of UnitDigit and then display it on LCD
}



// reverses a string 'str' of length 'len'
void reverse(char *str, int len)
{
    int i=0, j=len-1, temp;
    while (i<j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++; j--;
    }
}
 
 // Converts a given integer x to string str[].  d is the number
 // of digits required in output. If d is more than the number
 // of digits in x, then 0s are added at the beginning.
int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x)
    {
        str[i++] = (x%10) + '0';
        x = x/10;
    }
 
    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';
 
    reverse(str, i);
    str[i] = '\0';
    return i;
}
 
// Converts a floating point number to string.
void ftoa(float n, char *res, int afterpoint)
{
	
	int ipart,i;
	float fpart;
	//n=78.98;
	if(n<0){
		writedata('-');
		n=fabs(n);
	}
    // Extract integer part
    ipart = (int)n;
 
    // Extract floating part
    fpart = n - (float)ipart;
 
     i = intToStr(ipart, res, 0);
 
    
    if (afterpoint != 0)
    {
        res[i] = '.';  // add dot
 
        
        fpart = fpart * pow(10, afterpoint);
 
        intToStr((int)fpart, res + i + 1, afterpoint);
    }
		for(i=0;i<strlen(res);i++){
			writedata(res[i]);
		}
}
 




















