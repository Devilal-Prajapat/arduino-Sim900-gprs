#include <SoftwareSerial.h>
SoftwareSerial mySerial(5, 6); //Rx,TX

void setup()
{
  mySerial.begin(19200); // the GSM baud rate
  Serial.begin(9600); // the Serial baud rate
  delay(100);
}
void loop()
{
  Serial.println("Please enter s,r,d send ,receive and delete msg");
  if (Serial.available() > 0)
    switch (Serial.read())
    {
      case 't':
        textMsg();
        break;
      case 'r':
        recvMsg();
        break;
      case 'd':
        deleteMsg();
        break;
    }
}

void textMsg()
{
  mySerial.print("AT\r");
  delay(100);
  ShowSerialData();
  mySerial.print("AT+CMGF=1\r"); //Because we want to send the SMS in text mode
  delay(100);
  ShowSerialData();
  mySerial.println("AT + CMGS = \"+919479815814\"");//send sms message, be careful need to
  //add a country code before the cell phone number
  delay(100);
  ShowSerialData();
  mySerial.println("Hello How are u");
  delay(100);
  ShowSerialData();
  mySerial.println((char)26); //the ASCII code of the ctrl+z is 26
  delay(100);
  ShowSerialData();
  mySerial.println();
  delay(1000);
  ShowSerialData();
}
void recvMsg()
{
  mySerial.println("AT+CMGF=1\r");
  delay(200);
  ShowSerialData();
  mySerial.println("AT+CNMI=2,2,0,0,0"); indication of live msg
  delay(2000);
  ShowSerialData();
  mySerial.println("AT+CMGL=\"ALL\"\r"); // to read ALL the SMS in text mode
  delay(2000);
  ShowSerialData();
  mySerial.println("AT+CMGR=3\r");//  to read the SMS in text mode from location x
  delay(1000);
  ShowSerialData();
}

void deleteMsg()
{
  mySerial.println("AT+CMGD=1\r");// to delete the SMS in text mode from location x
  delay(2000);
  ShowSerialData();
  mySerial.println("AT+CMGD=0,4\r");// to delete the SMS in text mode from all location
  delay(2000);
  ShowSerialData();

}
void ShowSerialData()
{
  while (mySerial.available() != 0)
    Serial.write(mySerial.read());
}
