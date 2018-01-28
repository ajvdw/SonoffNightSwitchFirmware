const char home_html[] = R"=====(
<!-- header -->
<!-- menu -->
<script type=text/javascript>
xmlhttp=new XMLHttpRequest();
function update_status()
{ 
  xmlhttp.onreadystatechange=function()
  {
    if(xmlhttp.readyState==4&&xmlhttp.status==200)
    {
       var arr=JSON.parse(xmlhttp.responseText);
       document.getElementById("datetime").innerHTML=arr[0];
       document.getElementById("switch").innerHTML=arr[1];
    }
  }
  xmlhttp.open("GET","status",true);xmlhttp.send();
}
self.setInterval("update_status()",1000);
update_status();
</script>
<div id="content">
<h2>@title</h2>
<table>
<tr><th>Model:</th><td>@model</td></tr>
<tr><th>Firmware:</th><td>v@major.@minor</td></tr>
<tr><th>Date/Time:</th><td id=datetime></td></tr>
<tr><th>Switch:</th><td id=switch></td></tr>
</table>
</div>
<!-- footer -->
)=====";

void send_home_html()
{
  String html=home_html;
  Serial.println(__FUNCTION__); 

  html.replace( "<!-- header -->", html_header() );
  html.replace( "<!-- menu -->", html_menu() );  
  html.replace( "<!-- footer -->", html_footer() );  
  html.replace( "@title", "Status" );     
  html.replace( "@model", "Sonoff (Itead)" );
  html.replace( "@major", String(VERSION_MAJOR) );
  html.replace( "@minor", String(VERSION_MINOR) );
  server.send ( 200, "text/html", html );

}

void send_status_json()
{
  String data="[";
  if( TimeValid )
    data += String("\"") + DateTimeString() + "\",";
  else
    data += String("\"?\",");  
  data += String("\"") + switchString() + "\",";
  data += String("\"") + RunningString() + "\"";
  data += String("]");
  server.send ( 200, "application/json", data );
  Serial.println(__FUNCTION__); 
}


