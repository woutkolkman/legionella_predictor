#include "main.h"

void setup() {

  //start USART (serial monitor)
  Serial.begin(115200);

  //initialisation of WiFi
  // setup a wifi access point
  Serial.printf("Start a wifi accesspoint : %s \n",ssid);
  WiFi.softAP(ssid, password);

  // get ip address for esp32
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // start to listen to port 80
  Serial.println("Begin to listen to port 80");
  server.begin();

  //initialisation of LoRa
  Serial2.begin(9600);
  Serial.println("Starting Reader");

  // this init will set the pinModes for you
  Transceiver.init();

}

void loop() {

  WiFiClient client = server.available(); 

if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            //client.print("<h3>legionela predetector test server</h3>");
            //client.print("Click <a href=\"/H\">here</a> to turn the LED on pin 5 on.<br>");
            //client.print("Click <a href=\"/L\">here</a> to turn the LED on pin 5 off.<br>");
            client.print(webpage);
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(5, HIGH);               // GET /H turns the LED on
          
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(5, LOW);                // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }

  
  
  //THESE LINES HAVE TO BE IMPLEMENTED
  // if the transceiver serial is available, proces incoming data
  // you can also use Transceiver.available()
  if (Transceiver.available()) {
    LoRa_get_data();
  }
}

// ====================== code that later is needed to connect ESP32 on another wifi network
void scan_networks() { // function to scan local networks in area

  WiFi.disconnect(); // disconnect if previously disconnected

  uint8_t number_of_networks = WiFi.scanNetworks(); // scan different networks

  Serial.println(number_of_networks); 

  for (int i = 0; i < number_of_networks; i++) { // loop to present network name + signal strength + mac address
    Serial.print("Network name: ");
    Serial.println(WiFi.SSID(i));

    Serial.print("Signal strength: ");
    Serial.println(WiFi.RSSI(i));
    
    Serial.print("MAC address: ");
    Serial.println(WiFi.BSSIDstr(i));

    Serial.println("-----------------------");
  }
}

void connect_to_network() { // function to connect to current WiFi network

  WiFi.mode(WIFI_STA); // mode = station mode
  WiFi.begin(ssid, password); // initialize WiFi using networkname + password

  Serial.println();

  while (WiFi.status() != WL_CONNECTED) { // wait while connected has not been made yet
    Serial.print(".");
    delay(1000);
  }
  Serial.println(WiFi.localIP()); // show IP address if connection has been made
}

void LoRa_get_data() {
  // i highly suggest you send data using structures and not
    // a parsed data--i've always had a hard time getting reliable data using
    // a parsing method
  Transceiver.GetStruct(&MyData, sizeof(MyData));

  // dump out what was just received
  Serial.print("transmitter_ID: ");Serial.println(MyData.transmitter_ID);
  Serial.print("Hour: "); Serial.println(MyData.hour);
  Serial.print("Temp: "); Serial.println(MyData.Temperature);

}