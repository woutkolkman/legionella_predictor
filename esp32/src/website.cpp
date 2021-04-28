#include <Arduino.h>
#include "website.h"
#include <stdbool.h>
#include <iostream>
#include <string>
#include "main.h"
#include "config_wifi.h"

String new_sidd = "";
String new_password = "";
bool mode_is_hotspot = true;

// ----------- website html code fragments
// standard webpage head
const char webpage_head[] = {
    "<!DOCTYPE html>\n"
    "<html>\n"
	    "<head>\n"
		    "<title>legionella predetector setup</title>\n"
		    "<style>\n"
	    	    /* formmatting of title */
			    ".tittle {\n"
				     "background-color: lightblue;\n" 
				    "border: 5px double white;\n"
				    "text-align: center;\n"
			    "}"
          /* formatting of tables */
			    "#wifi table {\n"
				  "font-family: arial, sans-serif;\n"
				  "border-collapse: collapse;\n"
				  "width: 100%;\n"
			    "}\n"
			    "#wifi td, #wifi th {\n"
				    "border: 1px solid #dddddd;\n"
				    "text-align: left;\n"
				    "padding: 8px;\n"
			    "}\n"
			    "#wifi tr:nth-child(even) {\n"
				    "background-color: #dddddd;\n"
			    "}\n"
          ".info {\n"
				    "border: 1px solid blue;\n"
				    "text-align: center;\n"
			    "}\n"
		    "</style>\n"
	    "</head>\n\n"
	
	    "<body>\n"
		    "<div class=\"tittle\">\n"
			    "<h1>legionella predetector setup</h1>\n"
		    "</div>\n"
};

// standard website tail
const char webpage_tail[] = {
	    "</body>\n"
    "</html>\n"
}; 

// wifi form fragment
const char webpage_wifi_form[] = {
  "<!-- wifi config field -->\n"
		"<form action='/connectwifi' method='GET'>\n"
			"<fieldset>\n"
        "<legend>wifi settings</legend>\n"
				"<table>\n"
					"<tr>\n"
						"<th><label for='SSID'>SSID : </label></th>\n" 
						"<th><input id='SSID' type='text' name='SSID' required /></th>\n"
					"</tr>\n"
					
					"<tr>\n"
						"<th><label>password : </label></th>\n"
						"<th><input id='wifi_password' type='password' name='password' required /></th>\n"
					"</tr>\n"	
					"<tr><th /><th><input type='submit' name='submit' value='submit' /></th></tr>\n"
			"</fieldset>\n"
		"</form>\n"
};

// generate main page and send to client
void homepage() {
 String message = "";
  message += webpage_head;

  // show wifi status
  message += "<div class='info'>\n";
	message += "<p>" + ((mode_is_hotspot == false) ? ("Device connected to network " + new_sidd) : ("Device in hotspot mode.")) + "</p>\n";
	message += "</div>";

  message += "<p>click here to enter wifi credentials <a href='/scanwifi'>HERE</a></p>\n";
  message += webpage_tail;
	server.send(200, "text/html", message);
}

// generate page for invalid urls
void page_not_found() {
 String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

// scan for networks and print them on webpage
void scan_networks() {
 String message = "";
  message += webpage_head;

  // scan different networks
  uint8_t number_of_networks = WiFi.scanNetworks(); 

  // construct wifi table of avaible networks in html
  message += "<!-- wifi scan -->";
	message += "<table id=\"wifi\">";
	message +=   "<caption><strong>available wifi networks</strong></caption>";
	message +=   "<tr>";
  message +=     "<th>Network name</th>";
	message +=     "<th>Signal strength</th>";
	message +=  "</tr>";

  for (int i = 0; i < number_of_networks; i++) { // loop to present network name + signal strength + mac address
    message += "<tr><td>";
    message += WiFi.SSID(i) + "</td>";

    message += "<td>"; 
    char strength[10];
    itoa(WiFi.RSSI(i),strength,10);
    message += String(strength) + "</td>";

    message += "</tr>";
  }

  message += "</table>";

  // wifi form 
  message += webpage_wifi_form;

  message += webpage_tail;
  server.send(404, "text/html", message);
}

// connect to a wifi network when wifi credentials are given
void connect_to_network() {
 String message = "";
 bool connect_to_wifi = false;
 char char_sidd[80],char_password[80];
  // add head of webpage
  message += webpage_head;

  // check or the wifi credentials are post
  if(server.hasArg("SSID") && server.hasArg("password")) {
    new_sidd = server.arg("SSID");
    new_password = server.arg("password");
    message += "<h1> connecting with wifi network " + new_sidd + "</h1>";
    connect_to_wifi = true;
  }
  else {
    message += "<h1> there are no wifi credentials given!</h1>";
    message += "<p>click here to enter wifi credentials <a href='/scanwifi'>HERE</a></p>";
  }

  // add tail of webpage
  message += webpage_tail;
  server.send(404, "text/html", message);
  
  // if we get succesfull new wifi credentials
  if(connect_to_wifi == true)
  {
    new_sidd.toCharArray(char_sidd,80);
    new_password.toCharArray(char_password,80);
    setup_wifi(false,char_sidd,char_password);
  }
}
