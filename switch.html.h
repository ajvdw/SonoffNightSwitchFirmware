const char switch_html[] = R"=====(
<!-- header -->
<!-- menu -->
<div id="content">
<h2>@title</h2>
<form action="" method="post">
<table>
<tr><th width='200'>When:</th><td><select name="switchmode">@switchmode</select></td></tr>
<tr><th>Auto On:</th><td><select name="hoursafterdusk">@hoursafterdusk</select>:<select name="minutesafterdusk">@minutesafterdusk</select>&nbsp;after dusk</td></tr>
<tr><th>Auto Off:</th><td><select name="hoursbeforedawn">@hoursbeforedawn</select>:<select name="minutesbeforedawn">@minutesbeforedawn</select>&nbsp;before dawn</td></tr>
<tr><th></th><td><input type="submit" value="Save"></td></tr>
</table>
</form>
</div>
<!-- footer -->
)=====";

void send_switch_html()
{
  String modes[]={ "Always Off","Auto","Always On" };

	String html = switch_html;
  String options;
	 
	if (server.args() > 0 )  // Save Settings
	{
    if( !AdminEnabled ){
      send_error_html();
      return;
    }
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      if (server.argName(i) == "switchmode") config.switchmode=server.arg(i).toInt();
      if (server.argName(i) == "hoursafterdusk") config.hoursafterdusk=server.arg(i).toInt();
      if (server.argName(i) == "minutesafterdusk") config.minutesafterdusk=server.arg(i).toInt();
      if (server.argName(i) == "hoursbeforedawn") config.hoursbeforedawn=server.arg(i).toInt();      
      if (server.argName(i) == "minutesbeforedawn") config.minutesbeforedawn=server.arg(i).toInt();		
    }
    config.timestamp=timestamp;
		WriteConfig();
	}
  html.replace( "<!-- header -->", html_header() );
  html.replace( "<!-- menu -->", html_menu() );
  html.replace( "<!-- footer -->", html_footer() );
;
  
  html.replace("@title", "Switch");

  options=""; for( int i=0; i<3; i++ ){ options += (String)"<option " + (String)((config.switchmode==i)?"selected":"") + (String)" value='" + String(i) +(String)"'>" + modes[i] + "</option>"; }
  html.replace("@switchmode", options );  

  options=""; for( int i=0; i<6; i++ ){ options += (String)"<option " + (String)((config.hoursafterdusk==i)?"selected":"") + (String)" value='" + (String)i +(String)"'>" + (String)i + (String)"</option>"; }
  html.replace("@hoursafterdusk", options );  
  options=""; for( int i=0; i<60; i+=15 ){ options += (String)"<option " + (String)((config.minutesafterdusk==i)?"selected":"") + (String)" value='" + (String)i +(String)"'>" + ((i)?(String)i:(String)"00") + (String)"</option>"; }
  html.replace("@minutesafterdusk", options );   
  
  options=""; for( int i=0; i<6; i++ ){ options += (String)"<option " + (String)((config.hoursbeforedawn==i)?"selected":"") + (String)" value='" + (String)i +(String)"'>" + (String)i + (String)"</option>"; }
  html.replace("@hoursbeforedawn", options );  
  options=""; for( int i=0; i<60; i+=15 ){ options += (String)"<option " + (String)((config.minutesbeforedawn==i)?"selected":"") + (String)" value='" + (String)i +(String)"'>" + ((i)?(String)i:(String)"00") + (String)"</option>"; }
  html.replace("@minutesbeforedawn", options );   
  
   
	server.send ( 200, "text/html", html ); 
	Serial.println(__FUNCTION__); 
}
