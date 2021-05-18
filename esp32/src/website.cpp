#include <Arduino.h>
#include "website.h"
#include <stdbool.h>
#include <iostream>
#include <string>
#include "main.h"
#include "config_wifi.h"
#include <WebServer.h>

String new_sidd = "";
String new_password = "";

// ----------- website html code fragments
// standard webpage head
const char webpage_head[] = {
    "<!DOCTYPE html>\n"
    "<html>\n"
	    "<head>\n"
		    "<title>Legionella Predictor Setup</title>\n"
		    "<style>\n"
	    	    /* formmatting of title */
			    ".tittle {\n"
				     "background-color: lightblue;\n" 
				    "border: 5px double white;\n"
				    "text-align: center;\n"
			    "}"
          /* formatting of tables */
			    "#fancy table {\n"
				  "font-family: arial, sans-serif;\n"
				  "border-collapse: collapse;\n"
				  "width: 100%;\n"
			    "}\n"
			    "#fancy td, #fancy th {\n"
				    "border: 1px solid #dddddd;\n"
				    "text-align: left;\n"
				    "padding: 8px;\n"
			    "}\n"
			    "#fancy tr:nth-child(even) {\n"
				    "background-color: #dddddd;\n"
			    "}\n"
          ".info {\n"
				    "border: 1px solid blue;\n"
				    "text-align: center;\n"
			    "}\n"
		    "</style>\n"
	    "</head>\n\n"
	
	    "<a href='/'><body>\n"
		    "<div class=\"tittle\">\n"
			    "<h1>Legionella Predictor Setup</h1>\n"
		    "</div></a>\n"
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
					"<tr>"
            "<th><input formaction='/scanwifi' type='submit' name='scan_wifi_networks' value='scan for wifi networks' formnovalidate /></th>"
            "<th><input type='submit' name='submit' value='submit' /></th>"
          "</tr>\n"
        "</table>\n"
			"</fieldset>\n"
		"</form>\n"
};

const char cloud_setup_form[] = {
  "<!-- cloud setup form -->"
		"<form action='/configcloud' method='GET'>\n"
			"<fieldset>\n"
				"<legend>setup cloud connection</legend>\n"
				"<table>\n"
					"<tr>\n"
						"<th><label for='cloud_address'>Cloud address : </label></th>\n" 
						"<th><input id='cloud_address' type='text' name='cloud_address' required /></th>\n"
					"</tr>\n"
					"<tr>\n"
						"<th><label for='cloud_port'>Cloud port : </label></th>\n"
						"<th><input id='cloud_port' type='text' name='cloud_port' required /></th>\n"
					"</tr>\n"
					"<tr>\n"
						"<th><label for='cloud_path'>Cloud path : </label></th>\n"
						"<th><input id='cloud_path' type='text' name='cloud_path' required /></th>\n"
					"</tr>\n"
					"<tr><th /><th><input type='submit' name='submit' value='Connect to cloud' /></th></tr>\n"
				"</table>\n"
			"</fieldset>\n"
		"</form>\n"
};

// generate main page and send to client
void homepage() {
 String message = "";
  message += webpage_head;

  // show wifi status
  message += "<div class='info'>\n";
	message += "<p>" + ((settings.mode_is_hotspot == false) ? ("Device connected to network " + new_sidd) : ("Device in hotspot mode.")) + "</p>\n";
	message += "</div>";

  message += "<p>Click <a href='/scanwifi'><b>HERE</b></a> to enter wifi credentials.</p>\n";
  message += "<p>Click <a href='/configcloud'><b>HERE</b></a> to setup the cloud connection.</p>\n";
  message += webpage_tail;
	interface_server.send(200, "text/html", message);
}

// go to homepage when a invalid url is entered or trigger captive portal system
void page_not_found() {
  interface_server.sendHeader("Location", String("http://") + interface_server.client().localIP().toString(), true);
  interface_server.send(302, "text/plain", "");
  interface_server.client().stop();
}

// scan for networks and print them on webpage
void scan_networks() {
 String message = "";
  message += webpage_head;

  // if scan button is pressed, scan for availble wifi networks.
  if(interface_server.hasArg("scan_wifi_networks")) {
    // scan different networks
    uint8_t number_of_networks = WiFi.scanNetworks(); 

    // construct wifi table of avaible networks in html
    message += "<!-- wifi scan -->";
	  message += "<table id='fancy'>";
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
  }

  // wifi form 
  message += webpage_wifi_form;

  message += webpage_tail;
  interface_server.send(404, "text/html", message);
}

// connect to a wifi network when wifi credentials are given
void connect_to_network() {
 String message = "";
 bool connect_to_wifi = false;
  // add head of webpage
  message += webpage_head;

  // check or the wifi credentials are post
  if(interface_server.hasArg("SSID") && interface_server.hasArg("password")) {
    new_sidd = interface_server.arg("SSID");
    new_password = interface_server.arg("password");
    message += "<h1> connecting with wifi network " + new_sidd + "</h1>";
    connect_to_wifi = true;
  }
  else {
    message += "<h1> there are no wifi credentials given!</h1>";
    message += "<p>click here to enter wifi credentials <a href='/scanwifi'>HERE</a></p>";
  }

  // add tail of webpage
  message += webpage_tail;
  interface_server.send(404, "text/html", message);
  
  // if we get succesfull new wifi credentials
  if(connect_to_wifi == true) {
    // convert strings to char array and update settings
    new_sidd.toCharArray(settings.wifi_sidd,33);
    new_password.toCharArray(settings.wifi_password,64);
    settings.mode_is_hotspot = false;

    // connect with wifi network
    setup_wifi(settings.mode_is_hotspot,settings.wifi_sidd,settings.wifi_password);

    // save settings to EEPROM
    save_settings(&settings);
  }
}

// generate webpage for setup cloud settings
void config_cloud_page() {
 String message = ""; 
  // add html head 
  message += webpage_head;
 
  // check if submit button is pressed
  if(interface_server.hasArg("submit")) {
    // change cloud settings
    strcpy(settings.cloud_address, interface_server.arg("cloud_address").c_str());
    strcpy(settings.cloud_port, interface_server.arg("cloud_port").c_str());
    strcpy(settings.cloud_path, interface_server.arg("cloud_path").c_str());

    // save settings to EEPROM
    save_settings(&settings);

    // give a message that cloud settings are changed
    message += "<div class='info'>\n";
	  message += "<p>The cloud settings has changed.</p>\n";
	  message += "</div>\n";
  }

  // show current cloud settings
  message += "<table id='fancy'>\n";
  message += "<caption>Current cloud settings</caption>\n";
  message += "<tr><th><b>Cloud adress : </b></th><th>"+ String(settings.cloud_address) +"</th></tr>\n";
  message += "<tr><th><b>Cloud port   : </b></th><th>"+ String(settings.cloud_port) +"</th></tr>\n";
  message += "<tr><th><b>Cloud path   : </b></th><th>"+ String(settings.cloud_path) +"</th></tr>\n";
  message += "</table>\n";
 
  // a form to change cloud settings
  message += cloud_setup_form;

  // add html tail
  message += webpage_tail;
 
  // send generated html
  interface_server.send(200, "text/html", message);

}
