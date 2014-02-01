
#include <Wire.h>  
#include <Time.h>  
#define DS1307_ADDRESS 0x68
#define  dataPin  11
#define  clockPin 13
#define  loadPin  5
byte zero = 0x00;
const byte segFont[] =
{
  B00111111, B00000110, B01011011, B01001111,
  B01100110, B01101101, B01111100, B00000111,
  B01111111, B01100111, B01000000
};

int lastsecond = 0;

byte bChars[10] ;
int dChars[10] ;

byte seconds;
byte hours;
byte minutes;
void setup()
{
  pinMode(loadPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  Serial.begin(9600);
  Wire.begin();
  timetoIV18(12,75,34);
  printDate();
  RefreshIV18();

}

void loop(){

  printDate();
  timetoIV18(hours,minutes,seconds);

  if (lastsecond != seconds){
   lastsecond = seconds;
   dChars[1] = !dChars[1];
   dChars[4] = !dChars[4];
   dChars[7] = !dChars[7];
   timetoIV18(hours,minutes,seconds);
 }
 RefreshIV18();
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
  memset(bChars,0,sizeof(bChars));

  bChars[7]=h % 10;	
  bChars[8]=h / 10;
  
  bChars[6]=10;

  bChars[4]=m % 10;	
  bChars[5]=m / 10;

  bChars[3]=10;

  bChars[1]=s % 10;	
  bChars[2]=s / 10;

  

}


byte decToBcd(byte val){
// Convert normal decimal numbers to binary coded decimal
return ( (val/10*16) + (val%10) );
}
void printDate(){
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();

  Wire.requestFrom(DS1307_ADDRESS, 3);

  seconds = bcdToDec(Wire.read());
  minutes = bcdToDec(Wire.read());
  hours = bcdToDec(Wire.read() & 0b111111); //24 hour time
   /*
  int weekDay = bcdToDec(Wire.read()); //0-6 -> sunday - Saturday
  int monthDay = bcdToDec(Wire.read());
  int months = bcdToDec(Wire.read());
  int years = bcdToDec(Wire.read());
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

void serialEvent(){
   byte a = Serial.read();
   byte b = Serial.read();
   byte c = Serial.read();
   byte d = Serial.read();


  time_t t = (a<<24)+(b<<16)+(c<<8)+d;
  setDateTime(t);


}


void setDateTime(time_t t){

  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero); //stop Oscillator

  Wire.write(decToBcd(second(t)));
  Wire.write(decToBcd(minute(t)));
  Wire.write(decToBcd(hour(t)));
  Wire.write(decToBcd(weekday(t)));
  Wire.write(decToBcd(day(t)));
  Wire.write(decToBcd(month(t)));
  Wire.write(decToBcd(year(t)));

  Wire.write(zero); //start 

  Wire.endTransmission();

}
