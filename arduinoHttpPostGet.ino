#include <SoftwareSerial.h>
#include <String.h>
SoftwareSerial mySerial(5, 6); //rx,tx
int a = 0;
String name1 = "devilal";
int age = 25;
void setup()
{
  mySerial.begin(19200); // the GPRS baud rate
  Serial.begin(9600);
  delay(500);
}

void loop()
{
  serial.println("Please enter g,p,t to check response using get post and tcp");
  if (Serial.available() > 0) {
    char c = Serial.read()

    switch (c) {
      case 'p':
        httpPost();
        break;
      case 'g':
        httpGet();
        break;
      case 't':
        tcp();
        break;
      case 'd':
        break;
    }
  }
}
void ShowSerialData()
{
  while (mySerial.available() != 0)
    Serial.write(mySerial.read());
}

void httpPost()
{
  mySerial.println("AT\r");// to check the modem
  delay(100);
  mySerial.println("AT+CGATT?");//to check  whether gprs attached or not
  delay(1000);
  ShowSerialData();
  mySerial.println("AT+CGATT=1\r");// attach gprs services
  delay(1000);
  ShowSerialData();
  mySerial.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""); // set connection type gprs
  delay(1000);
  ShowSerialData();
  mySerial.println("AT+SAPBR=3,1,\"APN\",\"airtelgprs.com\""); // set the access point of service provider
  delay(1000);
  ShowSerialData();
  mySerial.println("AT+SAPBR=1,1");
  delay(2000);
  ShowSerialData();
  mySerial.println("AT+HTTPINIT"); //initialize the http connection
  delay(2000);
  ShowSerialData();
  mySerial.println("AT+HTTPPARA=\"CID\",1");
  delay(2000);
  ShowSerialData();
  mySerial.println("AT+HTTPPARA=\"URL\",\"http://www.devilalprjapt.com/gsm/insert.php?name=" + String(name1) + "&age=" + int(age) + "\"\r");
  delay(5000);
  ShowSerialData();
  mySerial.println("AT+HTTPACTION=0");//http action 0-GET,1-POST,3-HEAD
  delay(3000);
  ShowSerialData();
  mySerial.println("AT+HTTPREAD=0,1000");//start up the connection
  delay(15000);
  ShowSerialData();
  mySerial.println("AT+HTTPTERM"); // terminate the http connection
  delay(100);
  ShowSerialData();
  mySerial.println("AT+SAPBR=0,1");
}
void httpGet()
{
  mySerial.println("AT\r");// to check the modem
  delay(100);
  mySerial.println("AT+CGATT?");//to check  whether gprs attached or not
  delay(1000);
  ShowSerialData();
  mySerial.println("AT+CGATT=1\r");// attach gprs services
  delay(1000);
  ShowSerialData();
  mySerial.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""); // set connection type gprs
  delay(1000);
  ShowSerialData();
  mySerial.println("AT+SAPBR=3,1,\"APN\",\"airtelgprs.com\""); // set the access point of service provider
  delay(1000);
  ShowSerialData();
  mySerial.println("AT+SAPBR=1,1");
  delay(2000);
  ShowSerialData();
  mySerial.println("AT+HTTPINIT"); //initialize the http connection
  delay(2000);
  ShowSerialData();
  mySerial.println("AT+HTTPPARA=\"CID\",1");
  delay(2000);
  ShowSerialData();
  mySerial.println("AT+HTTPPARA=\"URL\",\"http://www.devilalprjapt.com/gsm/insert.php?name=" + String(name1) + "&age=" + int(age) + "\"\r");
  delay(5000);
  ShowSerialData();
  mySerial.println("AT+HTTPACTION=0");//http action 0-GET,1-POST,3-HEAD
  delay(3000);
  ShowSerialData();
  mySerial.println("AT+HTTPREAD=0,1000");//start up the connection
  delay(15000);
  ShowSerialData();
  mySerial.println("AT+HTTPTERM"); // terminate the http connection
  delay(100);
  ShowSerialData();
  mySerial.println("AT+SAPBR=0,1");
}
void tcp()
{
  mySerial.println("AT\r");// to check the modem
  delay(100);
  mySerial.println("AT+CGATT?");   //Attach or Detach from GPRS Service (Result  1 = Attach , 2 = Detached )
  delay(1000);
  ShowSerialData();
  mySerial.println("AT+CSTT=\"APN\"");//start task and setting the APN,
  delay(1000);
  ShowSerialData();

  mySerial.println("AT+CIICR");//bring up wireless connection
  delay(3000);
  ShowSerialData();

  mySerial.println("AT+CIFSR");//get local IP adress
  delay(2000);
  ShowSerialData();

  mySerial.println("AT+CIPSPRT=0");
  delay(3000);
  ShowSerialData();

  mySerial.println("AT+CIPSTART=\"TCP\",\"devilalprjapt.com\",\"80\"");//start up the connection
  delay(2000);
  ShowSerialData();

  Serial.println();
  mySerial.println("AT+CIPSEND");//begin send data to remote server
  delay(4500);
  ShowSerialData();

  mySerial.print("PUT /gsm/insert?");//here is the feed you apply from pachube
  delay(500);
  ShowSerialData();

  mySerial.print("name=Devilal");   //DATA feed name
  delay(10);
  ShowSerialData();

  mySerial.print("&age=26");   //DATA feed name
  delay(10);
  ShowSerialData();
  mySerial.print(" HTTP/1.1\r\n");
  delay(500);
  ShowSerialData();

  mySerial.print("Host: www.mysite.com\r\n");
  delay(500);
  ShowSerialData();
  mySerial.print("Connection: close");         //working as well
  mySerial.print("\r\n");
  mySerial.print("\r\n");
  delay(500);
  ShowSerialData();
  mySerial.println((char)26);//sending
  delay(5000);//waitting for reply, important! the time is base on the condition of internet
  mySerial.println();
  ShowSerialData();
  mySerial.println("AT+CIPCLOSE");//close the connection
  delay(100);
  ShowSerialData();
  mySerial.println("AT+CIPSHUT");
  delay(100);
}
