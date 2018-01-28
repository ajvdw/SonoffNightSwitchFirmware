const char solar_css[] = R"=====(
#footer,#header{background-color:#A55A79;color:#fff;text-align:center;padding:2px}
body{font-family:Arial,Verdana;}
h1{font-size:110%;}
h2{font-size:110%;}
th{text-align:left;font-size:90%;width:90px}
#menu{margin-top:1px;margin-bottom:1px;line-height:30px;background-color:#C69CAA;height:430px;width:100px;float:left;padding:15px}
#menu ul{list-style-type:none;margin:0;padding:0}
#menu li a{display:block;width:60px;color:#000;text-decoration:none}
#content{float:left;padding:10px;margin:15px}
#footer{clear:both}
)=====";

void send_css()
{
  server.send( 200, "text/css", solar_css );
  Serial.println(__FUNCTION__); 
}

