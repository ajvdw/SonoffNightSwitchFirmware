const char info_html[] = R"=====(
<!-- header -->
<!-- menu -->
<div id="content">
<h2>@title</h2>
<table>
<tr><th>SSID:</th><td>@x_ssid</td></tr>
<tr><th>IP:</th><td>@x_ip</td></tr>
<tr><th>Netmask:</th><td>@x_netmask</td></tr>
<tr><th>Gateway:</th><td>@x_gateway</td></tr>
<tr><th>Mac:</th><td>@x_mac</td></tr>
</table>
</div>
<!-- footer -->
)=====" ;

void send_info_html()
{
  String html=info_html;
  html.replace( "<!-- header -->", html_header() );
  html.replace( "<!-- menu -->", html_menu() );     
  html.replace( "<!-- footer -->", html_footer() ); 
  html.replace( "@title", "Network" );      
  html.replace( "@x_ssid", (String)WiFi.SSID() );
	html.replace( "@x_ip", (String) WiFi.localIP()[0] + "." +  (String) WiFi.localIP()[1] + "." +  (String) WiFi.localIP()[2] + "." + (String) WiFi.localIP()[3] );
	html.replace( "@x_gateway", (String) WiFi.gatewayIP()[0] + "." +  (String) WiFi.gatewayIP()[1] + "." +  (String) WiFi.gatewayIP()[2] + "." + (String) WiFi.gatewayIP()[3] );
	html.replace( "@x_netmask",  (String) WiFi.subnetMask()[0] + "." +  (String) WiFi.subnetMask()[1] + "." +  (String) WiFi.subnetMask()[2] + "." + (String) WiFi.subnetMask()[3] );
	html.replace( "@x_mac", (String)GetMacAddress() );
	server.send ( 200, "text/html", html );
	Serial.println(__FUNCTION__); 
}


