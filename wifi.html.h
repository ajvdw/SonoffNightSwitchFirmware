const char wifi_html[] = R"=====(
<!-- header -->
<!-- menu -->
<script>
function selssid(value)
{ 
  document.getElementById("ssid").value = value; 
};
var xmlhttp = new XMLHttpRequest();  
xmlhttp.onreadystatechange=function() 
{ 
  if (xmlhttp.readyState==4&&xmlhttp.status == 200) 
  {
    var arr = JSON.parse(xmlhttp.responseText);
    var i; var out = ''; 
    for(i = 0; i < arr.length; i+=2) 
      out += '<a href=javascript:selssid(\"'+arr[i]+'\")>'+arr[i+1]+'</a><br>'; 
    document.getElementById('ssids').innerHTML = out; 
  }
};
xmlhttp.open('GET', 'ssid', true); xmlhttp.send();
</script>
<div id="content">
<h2>@title</h2>
<table>
<form action="" method="post">
<tr><th>SSID:</th><td><input type="text" id="ssid" name="ssid" value="@ssid"></td></tr>
<tr><th>Password:</th><td><input type="password" id="password" name="password" value="@password"></td></tr>
<tr><th>DHCP:</th><td><input type="checkbox" id="dhcp" name="dhcp" value="on" @dhcp></td></tr>
<tr><th>IP:</th><td><input type="text" id="ip_0" name="ip_0" size="4" value="@ip_0">.<input type="text" id="ip_1" name="ip_1" size="4" value="@ip_1">.<input type="text" id="ip_2" name="ip_2" size="4" value="@ip_2">.<input type="text" id="ip_3" name="ip_3" value="@ip_3" size="4"></td></tr>
<tr><th>Netmask:</th><td><input type="text" id="nm_0" name="nm_0" size="4" value="@nm_0">.<input type="text" id="nm_1" name="nm_1" size="4" value="@nm_1">.<input type="text" id="nm_2" name="nm_2" size="4" value="@nm_2">.<input type="text" id="nm_3" name="nm_3" size="4" value="@nm_3"></td></tr>
<tr><th>Gateway:</th><td><input type="text" id="gw_0" name="gw_0" size="4" value="@gw_0">.<input type="text" id="gw_1" name="gw_1" size="4" value="@gw_1">.<input type="text" id="gw_2" name="gw_2" size="4" value="@gw_2">.<input type="text" id="gw_3" name="gw_3" size="4" value="@gw_3"></td></tr>
<tr><th></th><td><input type="submit" value="Save"></td></tr>
</form>
</table>
<br>
<table>
<tr><td id="ssids"></td></tr>
</table>
<br>
</div>
<!-- footer -->
)=====";

void send_wifi_html()
{
  String html = wifi_html;
  if (server.args() > 0 )  // Save Settings
  {
    if( !AdminEnabled )
    {
      send_error_html();
      return;
    }
    config.dhcp = false;
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      if (server.argName(i) == "ssid") config.ssid = urldecode(server.arg(i));
      if (server.argName(i) == "password") config.password = urldecode(server.arg(i)); 
      if (server.argName(i) == "ip_0") if (checkRange(server.arg(i))) 	config.IP[0] =  server.arg(i).toInt();
      if (server.argName(i) == "ip_1") if (checkRange(server.arg(i))) 	config.IP[1] =  server.arg(i).toInt();
      if (server.argName(i) == "ip_2") if (checkRange(server.arg(i))) 	config.IP[2] =  server.arg(i).toInt();
      if (server.argName(i) == "ip_3") if (checkRange(server.arg(i))) 	config.IP[3] =  server.arg(i).toInt();
      if (server.argName(i) == "nm_0") if (checkRange(server.arg(i))) 	config.Netmask[0] =  server.arg(i).toInt();
      if (server.argName(i) == "nm_1") if (checkRange(server.arg(i))) 	config.Netmask[1] =  server.arg(i).toInt();
      if (server.argName(i) == "nm_2") if (checkRange(server.arg(i))) 	config.Netmask[2] =  server.arg(i).toInt();
      if (server.argName(i) == "nm_3") if (checkRange(server.arg(i))) 	config.Netmask[3] =  server.arg(i).toInt();
      if (server.argName(i) == "gw_0") if (checkRange(server.arg(i))) 	config.Gateway[0] =  server.arg(i).toInt();
      if (server.argName(i) == "gw_1") if (checkRange(server.arg(i))) 	config.Gateway[1] =  server.arg(i).toInt();
      if (server.argName(i) == "gw_2") if (checkRange(server.arg(i))) 	config.Gateway[2] =  server.arg(i).toInt();
      if (server.argName(i) == "gw_3") if (checkRange(server.arg(i))) 	config.Gateway[3] =  server.arg(i).toInt();
      if (server.argName(i) == "dhcp") if (server.arg(i) == "on") config.dhcp = true;
    }
    WriteConfig();
    ConfigureWifi();
    AdminTimeOutCounter=120;
  }
  
  String state = "N/A";
  if (WiFi.status() == 0) state = "idle";
  else if (WiFi.status() == 1) state = "no ssid available";
  else if (WiFi.status() == 2) state = "scan completed";
  else if (WiFi.status() == 3) state = "connected";
  else if (WiFi.status() == 4) state = "connect failed";
  else if (WiFi.status() == 5) state = "connection lost";
  else if (WiFi.status() == 6) state = "disconnected";
  
  html.replace( "<!-- header -->", html_header() );
  html.replace( "<!-- menu -->", html_menu() );
  html.replace( "<!-- footer -->", html_footer() );
  html.replace( "@title", (String)"Wifi (" + state + ")");
  html.replace( "@ssid", (String) config.ssid );
  html.replace( "@password",  (String)( AdminEnabled? config.password: "undisclosed" ) ); 
  html.replace( "@ip_0",  (String)( config.dhcp ? WiFi.localIP()[0]:config.IP[0] ) );
  html.replace( "@ip_1",  (String)( config.dhcp ? WiFi.localIP()[1]: config.IP[1] ) );
  html.replace( "@ip_2",  (String)( config.dhcp ? WiFi.localIP()[2]: config.IP[2] ) );
  html.replace( "@ip_3",  (String)( config.dhcp ? WiFi.localIP()[3]: config.IP[3] ) );
  html.replace( "@nm_0",  (String)( config.dhcp ? WiFi.subnetMask()[0]: config.Netmask[0]) );
  html.replace( "@nm_1",  (String)( config.dhcp ? WiFi.subnetMask()[1]: config.Netmask[1]) );
  html.replace( "@nm_2",  (String)( config.dhcp ? WiFi.subnetMask()[2]: config.Netmask[2]) );
  html.replace( "@nm_3",  (String)( config.dhcp ? WiFi.subnetMask()[3]: config.Netmask[3]) );
  html.replace( "@gw_0",  (String)( config.dhcp ? WiFi.gatewayIP()[0] : config.Gateway[0]) );
  html.replace( "@gw_1",  (String)( config.dhcp ? WiFi.gatewayIP()[1] : config.Gateway[1]) );
  html.replace( "@gw_2",  (String)( config.dhcp ? WiFi.gatewayIP()[2] : config.Gateway[2]) );
  html.replace( "@gw_3",  (String)( config.dhcp ? WiFi.gatewayIP()[3] : config.Gateway[3]) );
  html.replace( "@dhcp",  (String)( config.dhcp ? "checked" : "") ) ;
  
  server.send ( 200, "text/html", html ); 
  
  Serial.println(__FUNCTION__); 
}

void send_ssid_json()
{
  int n = WiFi.scanNetworks();
  
  String data="[";
  
  for (int i = 0; i < n; ++i )
  {
    int quality=0;
    if(WiFi.RSSI(i) <= -100) quality = 0;
    else if(WiFi.RSSI(i) >= -50) quality = 100;
    else quality = 2 * (WiFi.RSSI(i) + 100);
    
    if( i ) data += ", ";
      data += "\"" + String(WiFi.SSID(i)) + "\", \""  +  String(WiFi.SSID(i)) + " (" + String(quality) +"%)" + String((WiFi.encryptionType(i) == ENC_TYPE_NONE)?"":"*") + "\"" ;
  } 
  data += "]";
  
  server.send ( 200, "application/json", data );
  Serial.println(__FUNCTION__); 
}

