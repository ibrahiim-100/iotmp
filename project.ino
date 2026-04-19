#include <LiquidCrystal.h>
LiquidCrystal lcd(6,7,5,4,3,2);
 
#include <SoftwareSerial.h>
SoftwareSerial mySerial(8,9);

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
      j = 0;  
    }
    if(k > timeout)
    {
      return 1;
    }
  }
  if(!strncmp(ex,res,strlen(ex)))
  {
    return 0;
  }
  else
  {
    i=0;
    goto sl;
  }
} 

char buff[200],k=0;
void upload(unsigned int s1,const char *s2,const char *s3,const char *s4);

const char* ssid     = "iotserver";
const char* password = "iotserver123";

// ThingSpeak API Key
const char* apiKey = "GBTNFNAJT68658S6";

void setup() 
{  
  pinMode(relay_fan,OUTPUT);
  pinMode(relay_light,OUTPUT);
  pinMode(smoke,INPUT);
  pinMode(fire,INPUT);
  pinMode(ldr,INPUT);
   
  digitalWrite(relay_fan,LOW);
  digitalWrite(relay_light,LOW);
  
  Serial.begin(9600);
  mySerial.begin(9600);
  
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
  lcd.print("T:");
  lcd.setCursor(8,0);
  lcd.print("G:");
  
  lcd.setCursor(0,1);
  lcd.print("F:");
  lcd.setCursor(6,1);
  lcd.print("L:");
}

char bf2[50];
int g=0,f=0,count=0,lc=0;
int gk=0;

char smokes[10]="OFF";
char fires[10]="OFF";
char ldrs[10]="Dark";

void loop() 
{
  sensors.requestTemperatures();
  tempc = sensors.getTempCByIndex(0);

  lcd.setCursor(2,0);convertl(tempc); delay(1000);

  if(digitalRead(smoke) == LOW)
  {
    lcd.setCursor(10,0);lcd.print("ON ");    
    strcpy(smokes,"ON");       
    digitalWrite(relay_fan, HIGH);
    upload(tempc,smokes,fires,ldrs);  
  }
  if(digitalRead(smoke) == HIGH)
  {
    lcd.setCursor(10,0);lcd.print("OFF");    
    strcpy(smokes,"OFF"); 
  }

  if(digitalRead(fire) == LOW)
  {
    lcd.setCursor(2,1);lcd.print("ON ");    
    strcpy(fires,"ON");
    digitalWrite(relay_fan, HIGH);
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
    delay(15000); // ThingSpeak requires minimum 15 sec between updates
  }
}

void upload(unsigned int s1, const char *s2, const char *s3, const char *s4)
{
  delay(2000);
  lcd.setCursor(15, 1); lcd.print("U");

  // Close any existing connection first
  mySerial.println("AT+CIPCLOSE=4");
  delay(1000);
  myserialFlush();

  // Connect to ThingSpeak over HTTP port 80
  mySerial.println("AT+CIPSTART=4,\"TCP\",\"api.thingspeak.com\",80");
  delay(8000);
  myserialFlush();

  // Build full GET request as one single string
  memset(buff, 0, sizeof(buff));
 sprintf(buff,
  "GET /update?api_key=%s&field1=%u&field2=%d&field3=%d&field4=%d HTTP/1.1\r\nHost: api.thingspeak.com\r\nConnection: close\r\n\r\n",
  apiKey, s1,
  (strcmp(s2,"ON")==0) ? 1 : 0,
  (strcmp(s3,"ON")==0) ? 1 : 0,
  (strcmp(s4,"Light")==0) ? 1 : 0
);

mySerial.print("temp");
mySerial.print(s1);
mySerial.print(" smokes");
mySerial.print(s2);
mySerial.print(" fires");
mySerial.print(s3);
mySerial.print(" ldrs");
mySerial.println(s4);  

  // Send length then data
  sprintf(bf2, "AT+CIPSEND=4,%u", strlen(buff));
  mySerial.println(bf2);
  delay(3000);
  myserialFlush();

  mySerial.print(buff);
  delay(5000);

  myserialFlush();
  mySerial.println("AT+CIPCLOSE=4");
  delay(1000);
  lcd.setCursor(15, 1); lcd.print(" ");
}

void wifiinit()
{
  char ret;  
  st:
  mySerial.println("ATE0");
  ret = check((char*)"OK",50);
  mySerial.println("AT");
  ret = check((char*)"OK",50);
  if(ret != 0)
  {
    delay(1000);
    goto st; 
  }
  
  lcd.clear();lcd.setCursor(0, 0);lcd.print("CONNECTING");  
  mySerial.println("AT+CWMODE=1");
  ret = check((char*)"OK",50);

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
  lcd.write(g);
  lcd.write(h);
}