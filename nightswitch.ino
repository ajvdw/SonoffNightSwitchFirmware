/* 
  Uitility Shield (c)2018 by A.J. van de Werken  
  Inspired by ESP_WebConfig by John Lassen. 
*/
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <TimeLib.h>  //by Paul Stoffregen, not included in the Arduino IDE !!!
#include <Timezone.h> //by Jack Christensen, not included in the Arduino IDE !!!
#include "Dusk2Dawn.h" // by DM Kishi, not using the github library because of some errors

#include "global.h"
#include "helpers.h"

// HTML
#include "css.h"
#include "favicon.ico.h"
#include "error.html.h"
#include "home.html.h"
#include "wifi.html.h"
#include "switch.html.h"
#include "info.html.h"
#include "timezone.html.h"
 
#define AdminTimeOut 300  // Defines the Time in Seconds, when the Admin-Mode will be diabled

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  EEPROM.begin(512);
  
  pinMode( FLASH_PIN, INPUT_PULLUP );
  pinMode( RED_PIN, OUTPUT );
  pinMode( GREEN_PIN, OUTPUT );
  pinMode( RELAY_PIN, OUTPUT );

  // Orange
  digitalWrite( RED_PIN, LEDON );
  digitalWrite( GREEN_PIN, LEDOFF );
  
  Serial.println("Starting nightswitch");

  if (!ReadConfig())
  {
    // DEFAULT CONFIG
    config.ssid = "MYSSID";
    config.password = "MYPASSWORD";
    config.dhcp = true;
    config.IP[0] = 192;config.IP[1] = 168;config.IP[2] = 1;config.IP[3] = 100;
    config.Netmask[0] = 255;config.Netmask[1] = 255;config.Netmask[2] = 255;config.Netmask[3] = 0;
    config.Gateway[0] = 192;config.Gateway[1] = 168;config.Gateway[2] = 1;config.Gateway[3] = 1;

    config.ntpServerName = "time.nist.gov";

    // Timezone / DST setting for Central Europe
    config.startweek=0;
    config.startday=0;
    config.startmonth=2;
    config.starthour=2;
    config.startminute=0;
    config.startoffset=14;
    config.endweek=0;
    config.endday=0;
    config.endmonth=9;
    config.endhour=3;
    config.endminute=0;
    config.endoffset=13; 

    config.lat = (long) (10000L * 52.37 ); // Amsterdam
    config.lon = (long) (10000L * 4.9 );

    config.hoursafterdusk=0;
    config.minutesafterdusk=0;
    config.hoursbeforedawn=0;      
    config.minutesbeforedawn=0;
    config.switchmode=0;
        
    WriteConfig();
    
    Serial.println("General config applied");
  }

  timestamp = config.timestamp;
  setTime( timestamp ); // better than nothing

  if (AdminEnabled)
  {
    WiFi.mode(WIFI_AP_STA);
    uint8_t mac[6];
    char apStr[18] = {0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0};
    WiFi.macAddress(mac);
    sprintf(apStr, "Shield-%02X%02X%02X%02X",  mac[2], mac[3], mac[4], mac[5]);
    WiFi.softAP( apStr , "nosecret" );
  }
  else
  {
    WiFi.mode(WIFI_STA);
  }
  
  ConfigureWifi();

  server.on ( "/", send_home_html );
  server.on ( "/favicon.ico", send_favicon_ico );
  server.on ( "/switch.css", send_css ); 
  server.on ( "/status", send_status_json );  
  server.on ( "/ssid", send_ssid_json );  
  server.on ( "/network", send_info_html );
  server.on ( "/switch", send_switch_html );
  server.on ( "/wifi", send_wifi_html );
  server.on ( "/time", send_tzdb_html );
   
  server.onNotFound ( []() { Serial.println("Page Not Found"); server.send ( 401, "text/html", "Page not Found" );   }  );
  server.begin();
  Serial.println( "HTTP server started" );

  AdminTimeOutCounter = AdminTimeOut;
        
  RebootTimecCounter =  86400 * 6; // Run at least for six days before reboot

	tkSecond.attach(1, Second_Tick);
  

  // Request NTP time
  int retry = 10;

  Serial.println("Starting UDP");
  udp.begin(localPort);  
  Serial.print("Local port: ");
  Serial.println(udp.localPort());
  while( !TimeValid && retry )
  {
    retry--;

    Serial.println("sending NTP packet...");
    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12]  = 49;
    packetBuffer[13]  = 0x4E;
    packetBuffer[14]  = 49;
    packetBuffer[15]  = 52;
  
    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    udp.beginPacket(config.ntpServerName.c_str(), 123); //NTP requests are to port 123
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    udp.endPacket();

    // wait to see if a reply is available
    delay(1000); 
    digitalWrite( GREEN_PIN, retry%2 ); 
    
    int cb = udp.parsePacket();
    if (!cb) 
    {
      Serial.println("no packet yet");
    }
    else 
    {
      Serial.print("packet received, length=");
      Serial.println(cb);
      // We've received a packet, read the data from it
      udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
  
      //the timestamp starts at byte 40 of the received packet and is four bytes,
      // or two words, long. First, esxtract the two words:
      unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
      // combine the four bytes (two words) into a long integer
      // this is NTP time (seconds since Jan 1 1900):
      unsigned long secsSince1900 = highWord << 16 | lowWord;
      // now convert NTP time into everyday time:
      Serial.print("Unix time = ");
      // subtract seventy years:
      timestamp = secsSince1900 - 2208988800UL;
      // print Unix time:
      Serial.println(timestamp);
 
      TimeValid = true;
     
      setTime( timestamp );
    }
  }


  
  Serial.println("Running ...");
}

void loop ( void ) 
{

      
	if( AdminEnabled )
	{
		if( AdminTimeOutCounter < 0 )
		{
			 AdminEnabled = false;
			 Serial.println("Admin Mode disabled!");
			 WiFi.mode(WIFI_STA);
		}
	}
  
  if( digitalRead( FLASH_PIN ) == LOW )
  {
    Serial.println("Rebooting...");
    reboot();       
  }

  int flash = now()%2;

  if( config.switchmode == 0 ) // always off
    switchstate = 0;
 
  if( config.switchmode == 2 ) // always on
    switchstate = 1;
  if( config.switchmode == 1 ) // Auto
  {
    Dusk2Dawn Here(0.0001 * config.lat, 0.0001*config.lon, 0 ); // UTC time
    int Sunrise  = Here.sunrise(year(now()), month(now()), day(now()), false) - config.hoursbeforedawn*60 - config.minutesbeforedawn;
    int Sunset   = Here.sunset(year(now()), month(now()), day(now()), false) + config.hoursafterdusk*60 + config.minutesafterdusk;  
    int minutesSinceMidnight = (now() % 86400) / 60;
    switchstate = ( minutesSinceMidnight > Sunrise &&  minutesSinceMidnight < Sunset )?0: 1;
  }

  if( switchstate )
    digitalWrite( RELAY_PIN, 1 );
  else
    digitalWrite( RELAY_PIN, 0 );
    
  if( TimeValid ) // Show State
  {
    if( config.switchmode == 1 ) // auto
    {
      if( switchstate )
      {
        // Flashing green
        digitalWrite( RED_PIN, LEDOFF );
        digitalWrite( GREEN_PIN, flash );
      }
      else
      { // Flashing red
        digitalWrite( RED_PIN, flash );
        digitalWrite( GREEN_PIN, LEDOFF );
      }
    }
    else
    {
      if( switchstate )
      {
        // Green
        digitalWrite( RED_PIN, LEDOFF );
        digitalWrite( GREEN_PIN, LEDON );
      }
      else
      { // Red
        digitalWrite( RED_PIN, LEDON );
        digitalWrite( GREEN_PIN, LEDOFF );
      }
    }
  }
  else
  {
    // Flashing Red/Green
    digitalWrite( RED_PIN, flash );
    digitalWrite( GREEN_PIN, !flash );
  }  
   
  if( RebootTimecCounter < 0 && SecondsToday() > 3600*4 && Weekday() == 0 ) 
  {   // Reboot after running for at least 6 days on Sunday, After 4AM
    reboot();
  }
	server.handleClient();
}

