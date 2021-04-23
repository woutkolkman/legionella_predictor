#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define POST_VALUE_LEN 8 //bytes per http parameter (5 numbers: -99.9...999.9)
#define PAYLOAD_SIZE 60  //* temperature values
#define POST_PAYLOAD_LEN ((POST_VALUE_LEN)*(PAYLOAD_SIZE))

const char *ssid = "hoi_simone";                //later laten invullen via website
const char *password = "hallo123";              //later laten invullen via website
char cloud_address[] = "http://145.44.235.205"; //later laten invullen via website
char cloud_port[] = "80";                       //later laten invullen via website
char cloud_path[] = "/packet";                  //later laten invullen via website
char post_payload[1+POST_PAYLOAD_LEN];
const char *transmitter_id = "XHD38SD3";        //vervangen door wat transmitter verzend


//function prototypes
bool send_to_cloud();
void connect_to_network();
void scan_networks();
void postData();


void setup() {
  Serial.begin(115200);

#if 0
  //genereer een test POST message met ?v=00001&v=00002&...
  post_payload[0] = '?';
  uint16_t j = 1;
  for (uint8_t i=0; i<PAYLOAD_SIZE; i++) {
    strncpy(&post_payload[j], "v=00000", 7);
    j += POST_VALUE_LEN;
    post_payload[j-1] = '&';
    post_payload[j-2] = (char) ((i%10)+'0');
    post_payload[j-3] = (char) ((i/10)+'0');
  }
  post_payload[j-1] = 0;
#else
  //genereer een test POST payload met elke regel een temperatuur
  strcpy(post_payload, transmitter_id);
  strcat(post_payload, "\n");
  char buff[5]; //'-','1','2','8','\0'
  for (uint8_t i=0; i<PAYLOAD_SIZE; i++) {
    strcat( post_payload, itoa(i,&buff[0],DEC) );
    strcat(post_payload, "\n");
  }
  /*
   * De waardes die aan het begin van post_payload staan zijn het oudste. 
   * Waardes worden gescheiden met '\n'. 
   * De eerste regel bevat het transmitter ID. 
   */
#endif

  connect_to_network();
  delay(500);
  send_to_cloud(); //verzend de testdata 1x nadat wifi is verbonden
}


void loop() {
  delay(10);

  if (!WiFi.isConnected()) {
    connect_to_network();
  }
}


bool send_to_cloud() {
  HTTPClient http;
  int data_length = (strlen(cloud_address) + (strlen(cloud_port)+1/*:*/) + strlen(cloud_path) /*+ POST_PAYLOAD_LEN*/);
  char server_url[1+data_length];
  bool retval = false;

  if (!WiFi.isConnected()) {
    Serial.println("Error in WiFi connection");
    return retval;
  }

  Serial.print("Data length: ");
  Serial.println(data_length);
  Serial.print("Sending to: ");
  strcpy(server_url, cloud_address);
  strcat(server_url, ":");
  strcat(server_url, cloud_port);
  strcat(server_url, cloud_path);
//  strcat(buffer, post_payload);
  Serial.write((uint8_t*) &server_url, sizeof(server_url));
  Serial.println();
  Serial.print("Payload: ");
  Serial.println(post_payload);

  http.begin(server_url);  //Specify destination for HTTP request
  http.addHeader("Content-Type", "text/plain");  //Specify content-type header
  int httpResponseCode = http.POST((uint8_t*) post_payload, POST_PAYLOAD_LEN); //Send the actual POST request

  if (httpResponseCode>0) {
    String response = http.getString(); //Get the response to the request
  
    Serial.println(httpResponseCode);   //Print return code
    Serial.println(response);           //Print request answer
    retval = true;
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }

  http.end(); //Free resources
  return retval;
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
  if (WiFi.isConnected()) {
    WiFi.disconnect();
  }

  WiFi.mode(WIFI_STA); // mode = station mode
  WiFi.begin(ssid, password); // initialize WiFi using networkname + password

  Serial.println();

  while (WiFi.status() != WL_CONNECTED) { // wait while connected has not been made yet
    Serial.print(".");
    delay(1000);
  }
  Serial.println();
  Serial.println(WiFi.localIP()); // show IP address if connection has been made
}


/*//TODO deze hele functie mag mogelijk weg
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
*/