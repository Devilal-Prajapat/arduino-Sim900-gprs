#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <string.h>

SoftwareSerial SIM800C(5, 6);
#define GSM_BUFF_SIZE   256
static char gsm_buff[GSM_BUFF_SIZE];
char imei[15];
uint16_t gsm_rx_index;

uint8_t send_flag = 1;
uint8_t check_response_flag = 0;
uint8_t new_sms_flag = 0;
uint8_t call_flag = 0;
uint32_t start_time;
typedef enum
{
  ATE0,
  AT_CPIN,
  AT_CMGF,
  AT_GSN,
  AT_CREG,
  AT_CGATT,
  AT_CGATT_SET,
  AT_CMGR,
  AT_CSCS,
  AT_CMGD,
  AT_CMGS,
  AT_SMS_STR,
} at_cmd_t;


void button(void)
{
  call_flag =  1;
}

void setup() {
  Serial.begin(9600); // the Serial baud rate
  pinMode(2,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), button, CHANGE);
  check_modem_ready();
  sim800_init();
  delete_sms();
  start_time = millis();
}

void loop() {
  if (Serial.available() > 0)
  {
    char c = Serial.read();
    if (c == 't')
    {
            send_sms();
//      send_text_message();
    }
    if (c == 'c')
    {
      make_call();
    }
  }

  if ((millis() - start_time) > 100)
  {
    start_time = millis();
    read_response();

    if (strstr(gsm_buff, "RING") > 0)
    {
      call_flag = 1;
      SIM800C.println("ATH\r\n");
    }
  }

  if (new_sms_flag)
  {
    new_sms_flag = 0;
    read_sms();

  }

  if (call_flag)
  {
    call_flag = 0;
    make_call();
  }

}

void check_modem_ready(void)
{
  SIM800C.begin(19200);
  do {
    SIM800C.println("AT\r");
    delay(1000);
    read_response();
    if (strstr(gsm_buff, "\r\nOK\r\n") > 0)
    {
      break;
    }
  } while (true);
}

void gsm_power_on(void)
{

}

void sim800_init(void)
{
  at_cmd_t at_cmd = ATE0;
  uint32_t time_out = 1000;
  uint8_t init_flag = 0;
  do
  {
    if (send_flag)
    {
      send_flag = 0;
      check_response_flag = 1;
      switch (at_cmd)
      {
        case ATE0:
          SIM800C.println("ATE0\r\n");
          break;

        case AT_CPIN:
          SIM800C.println("AT+CPIN?\r\n");
          break;

        case AT_GSN:
          SIM800C.println("AT+GSN\r\n");
          break;

        case AT_CMGF:
          SIM800C.println("AT+CMGF=1\r\n");
          break;

        case AT_CREG:
          SIM800C.println("AT+CREG?\r\n");
          break;

        default:
          break;
      }
    }

    if (check_response_flag)
    {
      delay(time_out);
      read_response();
      check_response_flag = 0;
      switch (at_cmd)
      {
        case ATE0:
          if (strstr(gsm_buff, "\r\nOK\r\n") > 0)
          {
            at_cmd = AT_CPIN;
          }
          break;

        case AT_CPIN:
          if (strstr(gsm_buff, "+CPIN: READY") > 0)
          {
            at_cmd = AT_GSN;
          }
          break;

        case AT_GSN:
          if (strstr(gsm_buff, "\r\nOK\r\n") > 0)
          {
            strncpy(imei, &gsm_buff[2], 15);
            Serial.println(imei);
            at_cmd = AT_CMGF;
          }
          break;

        case AT_CMGF:
          if (strstr(gsm_buff, "\r\nOK\r\n") > 0)
          {
            at_cmd = AT_CREG;
          }
          break;
        case AT_CREG:
          if (strstr((char *)gsm_buff, "\r\n+CREG: 0,1") > 0)
          {
            // home network
            init_flag = 1;
          }
          else if (strstr((char *)gsm_buff, "\r\n+CREG: 0,5") > 0)
          {
            // roaming network
            init_flag = 1;
          }
          break;
        default:
          break;
      }
      send_flag = 1;
    }
  } while (!init_flag);
}


/*
  void send_text_message(String mobileNumber, String m1) {
  String msg1 = String(m1) ;
  String sms_string = "AT+CMGS=\"" + mobileNumber + "\"\r";
  gsm.println("AT+CMGF=1");
  delay(1000);
  gsm.println(String(sms_string));
  delay(1000);
  gsm.println(String(msg1));
  delay(500);
  gsm.println((char)26);
  delay(1000);
  Serial.println("Msg send successfully...");
  }
  void recvMsg() {
  gsm.println("AT+CMGF=1\r");
  delay(200);
  show_serial_data();
  gsm.println("AT+CNMI=2,2,0,0,0"); //indication of live msg
  delay(2000);
  show_serial_data();
  gsm.println("AT+CMGL=\"ALL\"\r"); // to read ALL the SMS in text mode
  delay(2000);
  show_serial_data();
  gsm.println("AT+CMGR=3\r");//  to read the SMS in text mode from location x
  delay(1000);
  show_serial_data();
  }

  void deleteMsg() {
  gsm.println("AT+CMGD=1\r");// to delete the SMS in text mode from location x
  delay(2000);
  show_serial_data();
  gsm.println("AT+CMGD=0,4\r");// to delete the SMS in text mode from all location
  delay(2000);
  show_serial_data();
  }

  void show_serial_data() {
  while (gsm.available() != 0) {
    gsm.readBytesUntil('\r', buf, 128);
    break;
  }
  Serial.println(buf);
  memset(buf, 0, strlen(buf));
  }

  String process_gsm_data() {
  mobileNumber = "";
  String result = gsm_string;
  gsm_string = "";
  result.trim();
  Serial.println(result);
  if (result.startsWith("+CMT:")) {
    mobileNumber = result.substring(7, 20);
    Serial.println(mobileNumber);
    //send_text_message(String(mobileNumber),"100");
    int pos = result.indexOf("WWW");
    String res = result.substring(pos + 4);
    Serial.println(res);
    int pos1 = res.indexOf(",");
    String url = res.substring(0, pos1);
    Serial.println(url);
    eeprom_clear();
    delay(10);
    writeToEEPROM(0, url);
    delay(10);
    String str2 = res.substring(pos1 + 1);
    Serial.println(str2);
    int pos2 = str2.indexOf(",");
    String port = str2.substring(0, pos2);
    writeToEEPROM(20, port);
    delay(10);
    Serial.println(port);
    String apn = str2.substring(pos2 + 1);
    writeToEEPROM(40, apn);
    Serial.println(apn);
    String data = url + " " + port + " " + apn;
    send_text_message(mobileNumber, data);
    delay(100);
  }
  else if (result.startsWith("RING")) {
    gsm.println("ATH\r");
    //int pos = find_text("+91", result);
    int pos = result.indexOf("+91");
    mobileNumber = result.substring(pos, pos + 13);
    //Serial.print(mobileNumber);
    send_text_message(String(mobileNumber), "100");
    delay(100);
  }
  return mobileNumber;
  }

  void eeprom_clear() {
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
  }

  void writeToEEPROM(int e, String d) {
  const int BUFSIZE = 20;
  char buf[BUFSIZE];
  char myStringChar[BUFSIZE];
  d.toCharArray(myStringChar, BUFSIZE);
  strcpy(buf, myStringChar);
  EEPROM.put(e, buf);
  }

  String readFromEEPROM(int e) {
  const int BUFSIZE = 20;
  char buf[BUFSIZE];
  EEPROM.get(e, buf);
  Serial.println(buf);
  return String(buf);
  }
*/

void read_response(void)
{
  memset(gsm_buff, 0x00, GSM_BUFF_SIZE);
  gsm_rx_index = 0;
  if (SIM800C.available() > 0)
  {
    while (SIM800C.available() != 0) {
      gsm_buff[gsm_rx_index++] = SIM800C.read();
      if (gsm_rx_index > (GSM_BUFF_SIZE - 1))
      {
        break;
      }
    }
    if (strstr(gsm_buff, "+CMTI: ") > 0)
    {
      new_sms_flag = 1;
    }
    Serial.println(gsm_buff);
  }
}

void delete_sms(void)
{
  uint8_t error_count = 0;
  do {
    SIM800C.println("AT+CMGDA=\"DEL ALL\"\r\n");
    delay(2000);
    read_response();
    if (strstr(gsm_buff, "\r\nOK\r\n") > 0)
    {
      break;
    }
    else
    {
      error_count++;
      if (error_count > 3)
      {
        break;
      }
    }
  } while (true);
}

void read_sms(void)
{
  at_cmd_t at_cmd = AT_CMGF;
  uint8_t read_sms_done = 0;
  send_flag = 1;
  uint32_t time_out = 1000;
  do {
    if (send_flag)
    {
      send_flag = 0;
      check_response_flag = 1;
      switch (at_cmd)
      {
        case AT_CMGF:
          SIM800C.println("AT+CMGF=1\r\n");
          break;
        case AT_CMGR:
          SIM800C.println("AT+CMGR=1\r\n");
          break;
        case AT_CMGD:
          SIM800C.println("AT+CMGD=1\r\n");
          break;
        default:
          break;
      }
    }
    if (check_response_flag)
    {
      delay(time_out);
      read_response();
      check_response_flag = 0;
      switch (at_cmd)
      {
        case AT_CMGF:
          if (strstr((char*)gsm_buff, "\r\nOK\r\n") > 0)
          {
            at_cmd = AT_CMGR;
          }
          break;
        case AT_CMGR:
          if (strstr((char*)gsm_buff, "+CMGR: \"REC UNREAD\",") > 0)
          {

          }
          else if (strstr((char*)gsm_buff, "+CMGR: \"REC READ\",") > 0)
          {

          }
          at_cmd = AT_CMGD;
          break;
        case AT_CMGD:
          if (strstr((char*)gsm_buff, "\r\nOK\r\n") > 0)
          {
            read_sms_done = 1;
          }
          break;
        default:
          break;
      }
      send_flag = 1;
    }
  } while (!read_sms_done);
}

void make_call(void)
{
  char num[] = "<mobile number>";
  char call_buff[40];
  sprintf(call_buff, "ATD+91%s;\r\n", num);
  SIM800C.println(call_buff);
  delay(1000);
  unsigned long int start_time = millis();
  Serial.println(start_time);
  do
  {
    read_response();
    if (strstr(gsm_buff, "\r\nOK\r\n") > 0)
    {
     // break;
    } else
    {
      if (strstr(gsm_buff, "BUSY") > 0)
      {
//        SIM800C.println("ATH\r\n");
//        break;
      }
      else if (strstr(gsm_buff, "NO CARRIER") > 0)
      {
        SIM800C.println("ATH\r\n");
        break;
      }
    }
  }while(abs(millis()- start_time) < 10000);
  Serial.println(millis());
  Serial.println("Call_ended");
  SIM800C.println("ATH\r\n");
}

void send_sms(void) {
  char mob_num[] = "<mobile number>";
  char temp_str[128];
  uint8_t sms_send_done = 0;
  send_flag = 1;
  uint32_t time_out = 500;
  at_cmd_t at_cmd = AT_CMGF;
 SIM800C.flush();
  do {
    if (send_flag)
    {
      send_flag = 0;
      check_response_flag = 1;
      memset(temp_str, 0x00, 128);
      switch (at_cmd)
      {
        case AT_CMGF:
          SIM800C.println("AT+CMGF=1\r");
          break;
        case AT_CSCS:
          SIM800C.println("AT+CSCS=\"GSM\"\r");
          break;
        case AT_CMGS:
          Serial.println("AT_CMGS");
          sprintf(temp_str, "AT+CMGS=\"+91%s\"\r",mob_num);
          SIM800C.println((temp_str));
          break;
        case AT_SMS_STR:
        {
          Serial.println("AT_SMS_STR");
          SIM800C.write("THis is hell0");
          delay(500);
          SIM800C.write(char(26));
          time_out = 3000;
        }
          break;
        default:
          break;
      }
    }
    if (check_response_flag)
    {
      delay(time_out);
      read_response();
      SIM800C.flush();
      check_response_flag = 0;
      switch (at_cmd)
      {
        case AT_CMGF:
          if (strstr(gsm_buff, "\r\nOK\r\n") > 0)
          {
            at_cmd = AT_CSCS;
          }
        case AT_CSCS:
          if (strstr(gsm_buff, "\r\nOK\r\n") > 0)
          {
            at_cmd = AT_CMGS;
          }
        case AT_CMGS:
          if (strstr(gsm_buff, ">") > 0)
          {
            at_cmd = AT_SMS_STR;
          }
          break;

        case AT_SMS_STR:
          if (strstr(gsm_buff, "+CMGS: ") > 0)
          {
            sms_send_done = 1;
          }
          break;
      }
      send_flag = 1;
    }
  } while (!sms_send_done);
}

void send_text_message(void) {
  String msg1 = "Hello from sim800c\r";
  String sms_string = "AT+CMGS=\"<mbile num>\"\r";
  SIM800C.println("AT+CMGF=1\r");
  delay(500);
  read_response();
  SIM800C.println(String(sms_string));
  delay(500);
  read_response();
  SIM800C.println(String(msg1));
  delay(500);
  SIM800C.println((char)26);
  delay(1000);
  read_response();
  Serial.println("Msg send successfully...");
}
