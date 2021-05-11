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
  uint8_t transmitter_ID[TRANSMITTER_ID_SIZE];
  uint8_t Temperature[TEMPERATURE_SIZE];
} Temperatures;

void setup_wifi(bool hotspot,char *ssid, char *password);

// hotspot wifi credentials
const char *hotspot_ssid = "legionella_predictor(tiemen)";
const char *hotspot_password = "zeer_geheim2021";

// http post addresses
char cloud_address[80];
char cloud_port[10];
char cloud_path[80];


// init the ESP32
void setup() {
  //start USART (serial monitor)
  Serial.begin(115200);

  // setup default cloud settings
  strcpy(cloud_address, "http://145.44.235.205");
  strcpy(cloud_port, "80");
  strcpy(cloud_path,"/packet");

  // start wifi hotspot
  setup_wifi(true, (char *) hotspot_ssid,(char *) hotspot_password);

  // add webpages
  interface_server.on("/",homepage);
  interface_server.on("/scanwifi",scan_networks);
  interface_server.on("/connectwifi",connect_to_network);
  interface_server.on("/configcloud",config_cloud_page);
  interface_server.onNotFound(page_not_found);

  // start to listen to port 80
  Serial.println("HTTP server started on 80");
  interface_server.begin();

  //initialisation of LoRa
  Serial2.begin(9600);
  Serial.println("Starting Reader");

  // this init will set the pinModes for you
  Transceiver.init();
}


void loop() {
  static char post_payload[1+POST_PAYLOAD_LEN]; //will be filled with transmitter ID, temperature data, separated with '\n'

  interface_server.handleClient();
  delay(2);

  //THESE LINES HAVE TO BE IMPLEMENTED
  // if the transceiver serial is available, proces incoming data
  // you can also use Transceiver.available()
  if (Transceiver.available()) {
    LoRa_get_data();
    generate_http_post(&post_payload[0]);
    send_to_cloud(&post_payload[0]);
  }

  //TODO WiFi reconnect wanneer verbinding is verloren
  /*if (!WiFi.isConnected()) {
    connect_to_network();
  }*/
}


//put LoRa temperature values & transmitter ID in payload
void generate_http_post(char* payload) {

  /*
   * The values at the start of post_payload are the oldest. 
   * Values are separated with '\n'. 
   * The first line contains the transmitter ID. 
   */
  for (uint8_t i = 0; i < TRANSMITTER_ID_SIZE; i++) {
    payload[i] = Temperatures.transmitter_ID[i];
  }
  payload[TRANSMITTER_ID_SIZE] = 0;
  strcat(payload, "\n");

  char buff[5]; //'-','1','2','8','\0'
  for (uint8_t i=0; i<PAYLOAD_SIZE; i++) {
    strcat( payload, itoa(Temperatures.Temperature[i],&buff[0],DEC) );
    strcat(payload, "\n");
  }
}


//generates a HTTP POST message to cloud_address with the contents of payload
bool send_to_cloud(char* payload) {

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
  Serial.println(payload);

  http.begin(server_url);  //Specify destination for HTTP request
  http.addHeader("Content-Type", "text/plain");  //Specify content-type header
  int httpResponseCode = http.POST((uint8_t*) payload, POST_PAYLOAD_LEN); //Send the actual POST request

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
  Transceiver.get_struct(&Temperatures, sizeof(Temperatures));

  // dump out what was just received
  Serial.println("Temperatures: "); 
  for (int i = 0; i < TEMPERATURE_SIZE; i++) {
    Serial.print(i);
    Serial.print(" : ");
    Serial.print(Temperatures.Temperature[i]);
    Serial.println(" degrees.");
  }

  Serial.print("transmitter_ID: ");
  for (int i = 0; i < TRANSMITTER_ID_SIZE; i++) {
    Serial.print(Temperatures.transmitter_ID[i]);
    Serial.print(' ');
  }
}
