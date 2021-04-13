#include <Arduino.h>
#include <WiFi.h>

const char *ssid = "hoi_simone"; //later laten invullen via website
const char *password = "hallo123"; //later laten invullen via website
const char* hostGet = "mydatasite.com"; //later laten invullen via website


//function prototypes
void postData();
void connect_to_network();
void scan_networks();


void setup() {
  Serial.begin(115200); 

  connect_to_network();
}


void loop() {
//  postData();
  delay(10);
}


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


void postData() {

  WiFiClient clientGet;
  const int httpGetPort = 80;

  //the path and file to send the data to:
  String urlGet = "/data/collector.php";
 
  // We now create and add parameters:
  String src = "ESP";
  String typ = "flt";
  String nam = "temp";
  String vint = "92"; 

  urlGet += "?src=" + src + "&typ=" + typ + "&nam=" + nam + "&int=" + vint;
  Serial.print(">>> Connecting to host: ");
  Serial.println(hostGet);

  if (!clientGet.connect(hostGet, httpGetPort)) {
    Serial.print("Connection failed: ");
    Serial.print(hostGet);

  } else {
    clientGet.println("GET " + urlGet + " HTTP/1.1");
    clientGet.print("Host: ");
    clientGet.println(hostGet);
    clientGet.println("User-Agent: ESP8266/1.0");
    clientGet.println("Connection: close\r\n\r\n");

    unsigned long timeoutP = millis();
    while (clientGet.available() == 0) {
      if (millis() - timeoutP > 10000) {
        Serial.print(">>> Client Timeout: ");
        Serial.println(hostGet);
        clientGet.stop();
        return;
      }
    }

    //just checks the 1st line of the server response. Could be expanded if needed.
    while(clientGet.available()){
      String retLine = clientGet.readStringUntil('\r');
      Serial.println(retLine);
      break;
    }
  } //end client connection if else

  Serial.print(">>> Closing host: ");
  Serial.println(hostGet);
  clientGet.stop();
}
