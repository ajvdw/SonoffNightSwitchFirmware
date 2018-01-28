#ifndef GLOBAL_H
#define GLOBAL_H


ESP8266WebServer server(80);				// The Webserver
Ticker tkSecond;


volatile long AdminTimeOutCounter;  
volatile unsigned long RebootTimecCounter = 86400*5; // 5 days 
 
boolean AdminEnabled = true;		    // Enable Admin Mode for a given Time
bool TimeValid = false;
volatile unsigned long timestamp = 0;
volatile int switchstate = 0;

unsigned int localPort = 2390;      // local port to listen for UDP packets

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

#define LEDON  1
#define LEDOFF 0

const int RED_PIN   = 15; //14
const int GREEN_PIN = 12; //13

const int LED_PIN   = 2;   
const int FLASH_PIN = 0;
const int RELAY_PIN = 13; // 12

#define VERSION_MAJOR  2
#define VERSION_MINOR  4

struct strConfig {
	String ssid;
	String password;
	byte  IP[4];
	byte  Netmask[4];
	byte  Gateway[4];
	boolean dhcp;
  unsigned long timestamp;
  long lat; // * 10000
  long lon; // * 10000
  byte hoursafterdusk;
  byte minutesafterdusk;
  byte hoursbeforedawn; 
  byte minutesbeforedawn;
  byte switchmode;
  
  // ntpServer
  String ntpServerName;
  // Timezone, daylight saving
  byte startweek;
  byte startday;
  byte startmonth;
  byte starthour;
  byte startminute;
  byte startoffset;
  byte endweek;
  byte endday;
  byte endmonth;
  byte endhour;
  byte endminute;
  byte endoffset;
}   config;


#endif

