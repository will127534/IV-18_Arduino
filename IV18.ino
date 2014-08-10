#include <Arduino.h>

#include <Wire.h>  
#include <Time.h>  
#define DS1307_ADDRESS 0x68
#define  dataPin  11
#define  clockPin 13
#define  loadPin  5

byte zero = 0x00;
const byte segFont[] ={B00111111, B00000110, B01011011, B01001111,B01100110, B01101101, B01111100, B00000111,B01111111, B01100111, B01000000, B00000000};

int lastsecond = 0;

byte bChars[10] ;
int dChars[10] ;

byte seconds;
byte hours;
byte minutes;
int weekDay ; //0-6 -> sunday - Saturday
  int monthDay ;
  int months ;
  int years ;


void setup()
{
  pinMode(loadPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(A1, OUTPUT);
  digitalWrite(A1,HIGH);
  Serial.begin(9600);
  Wire.begin();
  timetoIV18(12,75,34);
  printDate();
  RefreshIV18();

}
int flag =0;
void loop(){
if(analogRead(A0)>55){
    flag =1;
}
while (flag){
  delay(100);
  if(analogRead(A0)<30){
        flag =0;
  }

}

  printDate();
//  datetoIV18(years,months,monthDay);
//for(int j=0;j<=1000;j++){
//RefreshIV18();
//}
 // printDate();
     timetoIV18(hours,minutes,seconds);
  //  for(int j=0;j<=1000;j++){
     RefreshIV18();
    // }


}
void serialEvent(){
int answer = Serial.read();
if (answer=='S'){
  Serial.read();
  setTime();
}
}

void setTime() {
  Serial.read();
  Serial.println("Please enter the current year, 00-99. - ");

  byte yearsett = readByteq();
  Serial.println("Please enter the current month, 1-12. - ");
  byte monthset = readByteq();
  //Serial.println(months[monthset-1]);
  Serial.println("Please enter the current day of the month, 1-31. - ");
  byte monthdayset = readByteq();
  //Serial.println(monthday);
  Serial.println("Please enter the current day of the week, 1-7.");
  Serial.println("1 Sun | 2 Mon | 3 Tues | 4 Weds | 5 Thu | 6 Fri | 7 Sat - ");
  byte weekdayset = readByteq();
  //Serial.println(days[weekday-1]);
  Serial.println("Please enter the current hour in 24hr format, 0-23. - ");
  byte hourset = readByteq();
  //Serial.println(hour);
  Serial.println("Please enter the current minute, 0-59. - ");
  byte minuteset = readByteq();
  //Serial.println(minute);
  Serial.println("Please enter the current second, 0-59. - ");
  byte secondset = readByteq();
  Serial.println("The data has been entered.");

  // The following codes transmits the data to the RTC
  Wire.beginTransmission(0x68);
  Wire.write(byte(0));
  Wire.write(decToBcd(secondset));
  Wire.write(decToBcd(minuteset));
  Wire.write(decToBcd(hourset));
  Wire.write(decToBcd(weekdayset));
  Wire.write(decToBcd(monthdayset));
  Wire.write(decToBcd(monthset));
  Wire.write(decToBcd(yearsett));
  Wire.write(byte(0));
  Wire.endTransmission();
  // Ends transmission of data
}


byte readByteq() {
  while (!Serial.available()) delay(10);
  byte reading = 0;
  byte incomingByte = Serial.read();
  while (incomingByte != '\n') {
    if (incomingByte >= '0' && incomingByte <= '9')
      reading = reading * 10 + (incomingByte - '0');
    else;
    incomingByte = Serial.read();
  }
  Serial.flush();
  return reading;
}
void RefreshIV18() 
{ 
  for (int count = 1;count<=9;count++){

    long data=0;
    data = segFont[bChars[count]];
    bitSet(data,17-count);
    if (dChars[count] == 1){
      bitSet(data,7);
    }
    digitalWrite(loadPin, LOW); 
  //Shiftin 20 bits
  for(byte i=0; i<20; i++) 
  { 
    //clockPin LOW
    digitalWrite(clockPin, LOW);
    //datapin bit to 1 OR 0 
    digitalWrite(dataPin,bitRead(data, i));
    //Set clockPin HIGH
    digitalWrite(clockPin, HIGH); 
  }

  digitalWrite(loadPin, HIGH);

}
}

byte bcdToDec(byte val)  {
// Convert binary coded decimal to normal decimal numbers
return ( (val/16*10) + (val%16) );
}


void timetoIV18(byte h,byte m,byte s)
{
  memset(bChars,0,sizeof(dChars));
  memset(dChars,0,sizeof(bChars));
  bChars[7]=h % 10; 
  bChars[8]=h / 10;
  bChars[6]=11;
  dChars[6]=1;

  bChars[4]=m % 10; 
  bChars[5]=m / 10;
 bChars[3]=11;
  dChars[3]=1;

  bChars[1]=s % 10; 
  bChars[2]=s / 10;

  

}
void datetoIV18(int yy,int mm,int dd)
{
  memset(dChars,0,sizeof(dChars));
  memset(bChars,0,sizeof(bChars));
 
  bChars[8]=yy/10;
  bChars[7]=yy%10; 
  
  bChars[6]=10;

  bChars[5]=mm/10; 
  bChars[4]=mm%10;

  bChars[3]=10;

  bChars[2]=dd / 10;
  bChars[1]=dd % 10; 

}


byte decToBcd(byte val){
// Convert normal decimal numbers to binary coded decimal
return ( (val/10*16) + (val%10) );
}

void printDate(){
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();

  Wire.requestFrom(DS1307_ADDRESS, 7);

  seconds = bcdToDec(Wire.read());
  minutes = bcdToDec(Wire.read());
  hours = bcdToDec(Wire.read() & 0b111111); //24 hour time
  
   weekDay = bcdToDec(Wire.read()); //0-6 -> sunday - Saturday
  monthDay = bcdToDec(Wire.read());
  months = bcdToDec(Wire.read());
   years = bcdToDec(Wire.read());
/*
  //print the date EG   3/1/11 23:59:59
  Serial.print(months);
  Serial.print("/");
  Serial.print(monthDay);
  Serial.print("/");
  Serial.print(years);
  Serial.print(" ");
  Serial.print(hours);
  Serial.print(":");
  Serial.print(minutes);
  Serial.print(":");
  Serial.println(seconds);
  */
}