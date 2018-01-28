#include "global.h"

String html_header()
{
  return (String) "<!DOCTYPE html>\n<html>\n<head>\n<link href='switch.css' rel=stylesheet>\n</head>\n<body>\n<div id='header'><h1>Night switch</h1></div>";
}

String html_menu()
{
  return (String) "<div id='menu'>\n<ul>\n"+
    "<li><a href='/'>Home</a></li>\n"+
    "<li><a href='/switch'>Switch</a></li>\n"+
    "<li><a href='/time'>Time</a></li>\n"+
    "<li><a href='/wifi'>Wifi</a></li>\n"+
    "<li><a href='/network'>Network</a></li>\n</ul>\n</div>";
}

String html_footer()
{
  return (String) "<div id='footer'>www.github.com/ajvdw</div>\n</body>\n</html>";
}

void WriteStringToEEPROM(int beginaddress, String string)
{
	char  charBuf[string.length()+1];
	string.toCharArray(charBuf, string.length()+1);
	for (int t=  0; t<sizeof(charBuf);t++)
	{
			EEPROM.write(beginaddress + t,charBuf[t]);
	}
}

String ReadStringFromEEPROM(int beginaddress)
{
  byte counter=0;
  char rChar;
  String retString = "";
  while (1)
  {
  	rChar = EEPROM.read(beginaddress + counter);
  	if (rChar == 0) break;
  	if (counter > 63) break;
  	counter++;
  	retString.concat(rChar);
  
  }
  return retString;
}

void EEPROMWritelong(int address, long value)
{
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);
  
  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}

long EEPROMReadlong(long address)
{
  //Read the 4 bytes from the eeprom memory.
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);
  
  //Return the recomposed long by using bitshift.
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void WriteConfig()
{
  Serial.println("Writing Config");
  EEPROM.write(0,'C');
  EEPROM.write(1,'F');
  EEPROM.write(2,'G');
  EEPROM.write(3,VERSION_MAJOR); 
  EEPROM.write(4,VERSION_MINOR); 
    
  EEPROM.write(16,config.dhcp);

  EEPROM.write(32,config.IP[0]);
  EEPROM.write(33,config.IP[1]);
  EEPROM.write(34,config.IP[2]);
  EEPROM.write(35,config.IP[3]);

  EEPROM.write(36,config.Netmask[0]);
  EEPROM.write(37,config.Netmask[1]);
  EEPROM.write(38,config.Netmask[2]);
  EEPROM.write(39,config.Netmask[3]);

  EEPROM.write(40,config.Gateway[0]);
  EEPROM.write(41,config.Gateway[1]);
  EEPROM.write(42,config.Gateway[2]);
  EEPROM.write(43,config.Gateway[3]);
  EEPROMWritelong(44,config.timestamp); // 4 Byte
  WriteStringToEEPROM(68,config.ssid);
  WriteStringToEEPROM(102,config.password);

  WriteStringToEEPROM(170,config.ntpServerName);   

  EEPROMWritelong(302,config.lat); // 4 Byte
  EEPROMWritelong(306,config.lon); // 4 Byte
  
  EEPROM.write(315,config.hoursafterdusk);
  EEPROM.write(316,config.minutesafterdusk);
  EEPROM.write(317,config.hoursbeforedawn);     
  EEPROM.write(318,config.minutesbeforedawn);
  EEPROM.write(319,config.switchmode);
        
  EEPROM.write(320, config.startweek);
  EEPROM.write(321, config.startday);
  EEPROM.write(322, config.startmonth);
  EEPROM.write(323, config.starthour);
  EEPROM.write(324, config.startminute);
  EEPROM.write(325, config.startoffset);
  EEPROM.write(326, config.endweek);
  EEPROM.write(327, config.endday);
  EEPROM.write(328, config.endmonth);
  EEPROM.write(329, config.endhour);
  EEPROM.write(330, config.endminute);
  EEPROM.write(331, config.endoffset); 


  EEPROM.commit();
}

boolean ReadConfig()
{
  
  Serial.println("Reading Configuration");
  if (EEPROM.read(0) == 'C' && EEPROM.read(1) == 'F'  && EEPROM.read(2) == 'G' &&  EEPROM.read(3) == VERSION_MAJOR )
  {
    Serial.println("Configurarion Found!");
    config.dhcp =   EEPROM.read(16);
    config.IP[0] = EEPROM.read(32);
    config.IP[1] = EEPROM.read(33);
    config.IP[2] = EEPROM.read(34);
    config.IP[3] = EEPROM.read(35);
    config.Netmask[0] = EEPROM.read(36);
    config.Netmask[1] = EEPROM.read(37);
    config.Netmask[2] = EEPROM.read(38);
    config.Netmask[3] = EEPROM.read(39);
    config.Gateway[0] = EEPROM.read(40);
    config.Gateway[1] = EEPROM.read(41);
    config.Gateway[2] = EEPROM.read(42);
    config.Gateway[3] = EEPROM.read(43);
    config.timestamp = EEPROMReadlong(44);
    config.ssid = ReadStringFromEEPROM(68);
    config.password = ReadStringFromEEPROM(102);
    config.ntpServerName = ReadStringFromEEPROM(170);    
    
    config.lat=EEPROMReadlong(302);;
    config.lon=EEPROMReadlong(306);;
        
    config.hoursafterdusk=EEPROM.read(315);;
    config.minutesafterdusk=EEPROM.read(316);
    config.hoursbeforedawn=EEPROM.read(317); 
    config.minutesbeforedawn=EEPROM.read(318);
    config.switchmode=EEPROM.read(319);
    
    config.startweek=EEPROM.read(320);
    config.startday=EEPROM.read(321);
    config.startmonth=EEPROM.read(322);
    config.starthour=EEPROM.read(323);
    config.startminute=EEPROM.read(324);
    config.startoffset=EEPROM.read(325);
    config.endweek=EEPROM.read(326);
    config.endday=EEPROM.read(327);
    config.endmonth=EEPROM.read(328);
    config.endhour=EEPROM.read(329);
    config.endminute=EEPROM.read(330);
    config.endoffset=EEPROM.read(331); 
     
    return true;
    
  }
  else
  {
    Serial.println("Configuration NOT FOUND!!!!");
    return false;
  }
}

void reboot()
{
    config.timestamp = now();
    // Save pulsecounter
    WriteConfig();
    
    ESP.reset();
}

// Check the Values is between 0-255
boolean checkRange(String Value)
{
   if (Value.toInt() < 0 || Value.toInt() > 255)
   {
     return false;
   }
   else
   {
     return true;
   }
}

// convert a single hex digit character to its integer value (from https://code.google.com/p/avr-netino/)
unsigned char h2int(char c)
{
    if (c >= '0' && c <='9'){
        return((unsigned char)c - '0');
    }
    if (c >= 'a' && c <='f'){
        return((unsigned char)c - 'a' + 10);
    }
    if (c >= 'A' && c <='F'){
        return((unsigned char)c - 'A' + 10);
    }
    return(0);
}

String urldecode(String input) // (based on https://code.google.com/p/avr-netino/)
{
   char c;
   String ret = "";
   
   for(byte t=0;t<input.length();t++)
   {
     c = input[t];
     if (c == '+') c = ' ';
         if (c == '%') {


         t++;
         c = input[t];
         t++;
         c = (h2int(c) << 4) | h2int(input[t]);
     }
    
     ret.concat(c);
   }
   return ret;
  
}

String GetMacAddress()
{
    uint8_t mac[6];
    char macStr[18] = {0};
    
    WiFi.macAddress(mac);
    sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0],  mac[1], mac[2], mac[3], mac[4], mac[5]);
    return  String(macStr);
}

void ConfigureWifi()
{
  Serial.println("Configuring Wifi");
  
  WiFi.begin (config.ssid.c_str(), config.password.c_str());
  if (!config.dhcp)
  {
    WiFi.config(IPAddress(config.IP[0],config.IP[1],config.IP[2],config.IP[3] ),  IPAddress(config.Gateway[0],config.Gateway[1],config.Gateway[2],config.Gateway[3] ) , IPAddress(config.Netmask[0],config.Netmask[1],config.Netmask[2],config.Netmask[3] ));
  }
}

unsigned long Running()
{
  return millis()/1000;
}

String RunningString()
{
  return (String) Running();
}

unsigned long SecondsToday()
{
  return  now() % 86400;
}

unsigned int Weekday()
{
  return (((now()/86400) + 4) % 7);  // Sunday is day 0
}


String DateTimeString()
{

  long startshift = config.startoffset - 12;
  if( startshift < 0 ) startshift = startshift * 60 - config.startminute; else startshift = startshift * 60 + config.startminute;
  
  long endshift = config.endoffset - 12;
  if( endshift < 0 ) endshift = endshift * 60 - config.endminute; else endshift = endshift * 60 + config.endminute;
     
  TimeChangeRule tcrDST = { "DST", config.startweek, config.startday+1, config.startmonth+1, config.starthour, startshift };
  TimeChangeRule tcrSTD = { "STD", config.endweek,   config.endday+1,   config.endmonth+1,   config.endhour,   endshift   };  
  Timezone TZ(tcrDST, tcrSTD);
  TimeChangeRule *tcr;        

  // convert to local timezone
  time_t l = TZ.toLocal(now(), &tcr);

  uint16_t _year=year(l), _month=month(l), _day=day(l), _hour=hour(l), _minute=minute(l), _second=second(l);
  return (String)(_year) + "-" + (String)((_month<10)?"0":"")+(String)(_month) + "-" + (String)((_day<10)?"0":"")+ (String)(_day) + "  " + (String)_hour + ":" + (String)((_minute<10)?"0":"") + (String)_minute + ":" + (String)((_second<10)?"0":"") + (String)_second;
}




String switchString()
{
  if( switchstate == 0 )
  {
    if( config.switchmode == 1)
      return "Off (auto)";
    else
      return "Off";
  }
  else
  {
    if( config.switchmode == 1)
      return "On (auto)";
    else
      return "On";
  }
}

void Second_Tick()
{
  AdminTimeOutCounter--; 
  RebootTimecCounter--;
}




