// shape of code inspired by https://github.com/espressif/arduino-esp32/blob/master/libraries/WebServer/examples/HelloServer/HelloServer.ino
#include "main.h"
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "website.h"
#include "config_wifi.h"
#include "ebyte.h"
#include <HTTPClient.h>

// set server to listen to port 80
WebServer interface_server(80);
EBYTE Transceiver(&Serial2, PIN_M0_, PIN_M1_, PIN_AX);
struct DATA {
  uint8_t transmitter_ID;
  unsigned long hour;
  int8_t Temperature;
} MyData;

void setup_wifi(bool hotspot,char *ssid, char *password);

// hotspot wifi credentials
const char *hotspot_ssid = "legionella_predictor";
const char *hotspot_password = "zeer_geheim2021";

// http post addresses
char cloud_address[] = "http://145.44.235.205"; //later laten invullen via website
char cloud_port[] = "80";                       //later laten invullen via website
char cloud_path[] = "/packet";                  //later laten invullen via website
char post_payload[1+POST_PAYLOAD_LEN];          //will be filled with transmitter ID, temperature data, separated with '\n'
const char *transmitter_id = "XHD38SD3";        //vervangen door wat transmitter verzend


// init the ESP32
void setup() {
  //start USART (serial monitor)
  Serial.begin(115200);

  // start wifi hotspot
  setup_wifi(true, (char *) hotspot_ssid,(char *) hotspot_password);

  // add webpages
  interface_server.on("/",homepage);
  interface_server.on("/scanwifi",scan_networks);
  interface_server.on("/connectwifi",connect_to_network);
  interface_server.onNotFound(page_not_found);

  // start to listen to port 80
  Serial.println("HTTP server started on 80");
  interface_server.begin();

  //initialisation of LoRa
  Serial2.begin(9600);
  Serial.println("Starting Reader");

  // this init will set the pinModes for you
  Transceiver.init();

  //genereer een test POST payload met elke regel een temperatuur (deze code moet nog worden weggehaald)
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
   * Deze blok code voor testwaardes komt niet in de eindversie, maar de inhoud 
   * van post_payload wordt gegenereerd tijdens het ontvangen van LoRa data. 
   */
}


void loop() { 
  interface_server.handleClient();
  delay(2);

  //THESE LINES HAVE TO BE IMPLEMENTED
  // if the transceiver serial is available, proces incoming data
  // you can also use Transceiver.available()
  if (Transceiver.available()) {
    LoRa_get_data();
  }

  //testcode cloud
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'c')
      send_to_cloud();
  }

  //TODO WiFi reconnect wanneer verbinding is verloren
  /*if (!WiFi.isConnected()) {
    connect_to_network();
  }*/
}


/*
 * This function generates a HTTP POST message to cloud_address with the contents of post_payload. 
 */
bool send_to_cloud() {

  HTTPClient http;
  int data_length = (strlen(cloud_address) + (strlen(cloud_port)+1/*:*/) + strlen(cloud_path));
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
