#include <LiquidCrystal.h>
LiquidCrystal lcd(6,7,5,4,3,2);
 
#include <SoftwareSerial.h>
SoftwareSerial mySerial(8,9);

//#include <Wire.h>
//#include "dht.h"


char res[130];

int relay_fan    = 10;
int relay_light  = 11;

int smoke = 12;
int fire  = 13;
int ldr   = A1;


/*********************** DS18B20 ***************************/
#include <OneWire.h> 
#include <DallasTemperature.h>
#define ONE_WIRE_BUS A0 
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);
/***********************************************************/


float tempc=0;

void serialFlush()
{
  while(Serial.available() > 0) 
  {
    char t = Serial.read();
  }
} 

void myserialFlush()
{
  while(mySerial.available() > 0) 
  {
    char t = mySerial.read();
  }
} 


char check(char* ex,int timeout)
{
  int i=0;
  int j = 0,k=0;
  while (1)
  {
    sl:
    if(mySerial.available() > 0)
    {
      res[i] = mySerial.read();
      if(res[i] == 0x0a || res[i]=='>' || i == 100)
      {
        i++;
        res[i] = 0;break;
      }
      i++;
    }
    j++;
    if(j == 30000)
    {
      k++;
     // Serial.println("kk");
      j = 0;  
    }
    if(k > timeout)
    {
      //Serial.println("timeout");
      return 1;
     }
  }//while 1
  if(!strncmp(ex,res,strlen(ex)))
  {
   // Serial.println("ok..");
    return 0;
   }
  else
  {
   // Serial.print("Wrong  ");
   // Serial.println(res);
    i=0;
    goto sl;
   }
} 

char buff[200],k=0;
void upload(unsigned int s1,const char *s2,const char *s3,const char *s4)
char readserver(void);
void clearserver(void);

const char* ssid = "iotserver";
const char* password = "iotserver123";



void setup() 
{  
  char ret;  

  pinMode(relay_fan,OUTPUT);
  pinMode(relay_light,OUTPUT);
  pinMode(smoke,INPUT);
  pinMode(fire,INPUT);
  pinMode(ldr,INPUT);
   
  digitalWrite(relay_fan,LOW);
  digitalWrite(relay_light,LOW);

  
  Serial.begin(9600);
  mySerial.begin(9600);
  
  //26.  IOT based industrial safety system using Arduino
  lcd.begin(16,2);
  lcd.clear();
  lcd.setCursor(0, 0);lcd.print("IOT Based Indust");
  lcd.setCursor(0, 1);lcd.print("rial Safety sys");   
     delay(2500);
  lcd.clear();
  lcd.setCursor(0, 0);lcd.print(" Using Arduino");
     delay(2500);   

  sensors.begin(); delay(1000);

   wifiinit();
  delay(2500);
  
  lcd.clear();
  lcd.print("T:");//2-3-4,0
  lcd.setCursor(8,0);
  lcd.print("G:");//8-9-10,0
  
  lcd.setCursor(0,1);
  lcd.print("F:");//2,1
  lcd.setCursor(6,1);
  lcd.print("L:");//8,1
}

char bf3[50];
int g=0,f=0,count=0,lc=0;
int gk=0;

char smokes[10];
char fires[10];
char ldrs[10];

void loop() 
{
 char ctrl=0;

    //tempc = analogRead(A0);
    //tempc = (tempc*0.48828125);
    
    sensors.requestTemperatures();
    tempc = sensors.getTempCByIndex(0);
    //tempf = ((tempc*1.8) + 32); 
    //lcd.setCursor(2,1);convertl(tempf);

    lcd.setCursor(2,0);convertl(tempc);    delay(1000);

    memset(smokes,'\0',strlen(smokes));
    if(digitalRead(smoke) == LOW)
      {
       lcd.setCursor(10,0);lcd.print("ON ");    
       strcpy(smokes,"ON");       
	           digitalWrite(relay_fan, HIGH);
       //upload1(2,"ON");
       
            upload(tempc,smokes,fires,ldrs);  
      }
    if(digitalRead(smoke) == HIGH)
      {
       lcd.setCursor(10,0);lcd.print("OFF");    
       strcpy(smokes,"OFF"); 
      }

     memset(fires,'\0',strlen(fires));
    if(digitalRead(fire) == LOW)
      {
       lcd.setCursor(2,1);lcd.print("ON ");    
       strcpy(fires,"ON");
	           digitalWrite(relay_fan, HIGH);
       //upload1(3,"ON");
       
            upload(tempc,smokes,fires,ldrs);  
      }
    if(digitalRead(fire) == HIGH)
      {
       lcd.setCursor(2,1);lcd.print("OFF");    
       strcpy(fires,"OFF");
      }

    if(tempc < 50 && digitalRead(smoke) == HIGH && digitalRead(fire) == HIGH)
      {
        digitalWrite(relay_fan, LOW);
      }
    else
      {
        digitalWrite(relay_fan, HIGH);  
      }

    memset(ldrs,'\0',strlen(ldrs));    
    if(digitalRead(ldr) == LOW)
      {
       lcd.setCursor(8,1);lcd.print("Light");    
       strcpy(ldrs,"Light");
       digitalWrite(relay_light, LOW);
      }
    if(digitalRead(ldr) == HIGH)
      {
       lcd.setCursor(8,1);lcd.print("Dark ");    
       strcpy(ldrs,"Dark");
       digitalWrite(relay_light, HIGH);
      }
        
      delay(1000);
       
      count++;
      lcd.setCursor(14,1);convertk(count);
  
      if(count > 30)
        {
         count = 0;
          delay(1000);
         
            upload(tempc,smokes,fires,ldrs);  
         
          delay(10000); 
        }
      ctrl=0;
      
}
char bf2[50];
void upload(unsigned int s1,const char *s2,const char *s3,const char *s4)
{
  delay(2000);
  lcd.setCursor(15, 1);lcd.print("U");
  myserialFlush();
  mySerial.println("AT+CIPSTART=4,\"TCP\",\"projectsfactoryserver.in\",80");
    
  //http://projectsfactoryserver.in/storedata.php?name=pf5&s1=25&s2=35
  //sprintf(buff,"GET http://embeddedspot.top/iot/storedata.php?name=iot139&s1=%u&s2=%u&s3=%u\r\n\r\n",s1,s2);
  
      delay(8000);
     
      
      memset(buff,0,strlen(buff));   
      sprintf(buff,"GET http://projectsfactoryserver.in/storedata.php?name=iot1892&s1=%u&s2=%s&s3=%s&s4=%s\r\n\r\n",s1,s2,s3,s4);
//      buff = buff + moss + "\r\n\r\n";
     // strcat(buff,s3);
         
      myserialFlush();
      sprintf(bf2,"AT+CIPSEND=4,%u",strlen(buff));
      mySerial.println(bf2);
      
         delay(5000);
          
          
          myserialFlush();
          mySerial.print(buff);
         
              delay(2000);
              
              mySerial.println("AT+CIPCLOSE");
       lcd.setCursor(15, 1);lcd.print(" ");  
}

void upload1(int s1,const char *s2)
{
  delay(2000);
  lcd.setCursor(15, 1);lcd.print("U");
  myserialFlush();
  mySerial.println("AT+CIPSTART=4,\"TCP\",\"projectsfactoryserver.in\",80");
    
  //http://projectsfactoryserver.in/storedata.php?name=pf5&s1=25&s2=35
  //sprintf(buff,"GET http://embeddedspot.top/iot/storedata.php?name=iot139&s1=%u&s2=%u&s3=%u\r\n\r\n",s1,s2);
  
      delay(8000);
     
      
      memset(buff,0,strlen(buff));
      
      if(s1 == 2){sprintf(buff,"GET http://projectsfactoryserver.in/storedata.php?name=iot1892&s2=%s\r\n\r\n",s2);}
      if(s1 == 3){sprintf(buff,"GET http://projectsfactoryserver.in/storedata.php?name=iot1892&s3=%s\r\n\r\n",s2);}

     // buff = buff + moss + "\r\n\r\n";
     // strcat(buff,s3);
         
      myserialFlush();
      sprintf(bf2,"AT+CIPSEND=4,%u",strlen(buff));
      mySerial.println(bf2);
      
         delay(5000);
          
          
          myserialFlush();
          mySerial.print(buff);
         
              delay(2000);
              
              mySerial.println("AT+CIPCLOSE");
       lcd.setCursor(15, 1);lcd.print(" ");  
}

char readserver(void)
{
  char t;
  delay(2000);
  lcd.setCursor(15, 1);lcd.print("R");
  myserialFlush();
  mySerial.println("AT+CIPSTART=4,\"TCP\",\"projectsfactoryserver.in\",80");

  //http://projectsfactoryserver.in/last.php?name=amvi001L

      delay(8000);
      memset(buff,0,strlen(buff));
      sprintf(buff,"GET http://projectsfactoryserver.in/last.php?name=iot17L\r\n\r\n");
      myserialFlush();
      sprintf(bf2,"AT+CIPSEND=4,%u",strlen(buff));
      mySerial.println(bf2);
      
         delay(5000);
          
          
          myserialFlush();
          mySerial.print(buff);
          
       //read status
        while(1)
        {
           while(!mySerial.available());
            t = mySerial.read();
           // Serial.print(t);
            if(t == '*' || t == '#')
            {
              if(t == '#')return 0;
              while(!mySerial.available());
               t = mySerial.read();
             //  Serial.print(t);
               delay(1000);
                myserialFlush();
               return t;
            }
        }
              delay(2000);
              
       mySerial.println("AT+CIPCLOSE");
       lcd.setCursor(15, 1);lcd.print(" "); 
       delay(2000);  
return t;
}

void clearserver(void)
{
  delay(2000);
  lcd.setCursor(15, 1);lcd.print("C");
  myserialFlush();
  mySerial.println("AT+CIPSTART=4,\"TCP\",\"projectsfactoryserver.in\",80");

  //sprintf(buff,"GET http://projectsfactoryserver.in/storedata.php?name=iot1&s10=0\r\n\r\n");
      delay(8000);
      memset(buff,0,strlen(buff));
      sprintf(buff,"GET http://projectsfactoryserver.in/storedata.php?name=iot1&s10=0\r\n\r\n");
      myserialFlush();
      sprintf(bf2,"AT+CIPSEND=4,%u",strlen(buff));
      mySerial.println(bf2);
      
         delay(5000);
          
          
          myserialFlush();
          mySerial.print(buff);
          
          delay(2000);
          myserialFlush();
      
              
              
       mySerial.println("AT+CIPCLOSE");
       lcd.setCursor(15, 1);lcd.print(" "); 
       delay(2000);  
}


void wifiinit()
{
 char ret;  
  st:
  mySerial.println("ATE0");
  ret  = check((char*)"OK",50);
  mySerial.println("AT");
  ret  = check((char*)"OK",50);
  if(ret != 0)
  {
    delay(1000);
   goto st; 
  }
  
     lcd.clear();lcd.setCursor(0, 0);lcd.print("CONNECTING");  
  mySerial.println("AT+CWMODE=1");
   ret  = check((char*)"OK",50);
 cagain:
    
  myserialFlush();
  mySerial.print("AT+CWJAP=\"");
  mySerial.print(ssid);
  mySerial.print("\",\"");
  mySerial.print(password);
  mySerial.println("\"");
  if(check((char*)"OK",300))goto cagain;    
  mySerial.println("AT+CIPMUX=1");
  delay(1000);
 

  lcd.clear();lcd.setCursor(0, 0);lcd.print("WIFI READY"); 
}

void convertl(unsigned int value)
{
  unsigned int a,b,c,d,e,f,g,h;

      a=value/10000;
      b=value%10000;
      c=b/1000;
      d=b%1000;
      e=d/100;
      f=d%100;
      g=f/10;
      h=f%10;


      a=a|0x30;               
      c=c|0x30;
      e=e|0x30; 
      g=g|0x30;              
      h=h|0x30;
    
     
  // lcd.write(a);
  // lcd.write(c);
   lcd.write(e); 
   lcd.write(g);
   lcd.write(h);
}
void convertk(unsigned int value)
{
  unsigned int a,b,c,d,e,f,g,h;

      a=value/10000;
      b=value%10000;
      c=b/1000;
      d=b%1000;
      e=d/100;
      f=d%100;
      g=f/10;
      h=f%10;


      a=a|0x30;               
      c=c|0x30;
      e=e|0x30; 
      g=g|0x30;              
      h=h|0x30;
    
     
  // lcd.write(a);
  // lcd.write(c);
  // lcd.write(e); 
   lcd.write(g);
   lcd.write(h);
}
