const char error_html[] = R"=====(
<!-- header -->
<!-- menu -->
<div id="content">
<h2>@title</h2>
<table>
<tr><th>@error</th></tr>
</table>
</div>
<!-- footer -->
)=====" ;

void send_error_html()
{
  String html=error_html;
  html.replace( "<!-- header -->", html_header() );
  html.replace( "<!-- menu -->", html_menu() ); 
  html.replace( "<!-- footer -->", html_footer() );  
  html.replace( "@title", "Error" );
  html.replace( "@error", "Admin mode disabled" );
	
	server.send ( 200, "text/html", html );
	Serial.println(__FUNCTION__); 
}


