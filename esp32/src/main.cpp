#include "main.h"

// --- globals
settings_t settings;
WebServer interface_server(80);
EBYTE Transceiver(&Serial2, PIN_M0_, PIN_M1_, PIN_AX);

struct DATA {
  uint8_t transmitter_ID[TRANSMITTER_ID_SIZE];
  uint8_t Temperature[TEMPERATURE_SIZE];
} Temperatures;

// init the ESP32
void setup() {
  //start USART (serial monitor)
  Serial.begin(115200);

  // load settings if EEPROM data is valid
  Serial.println("Load last known settings:");
  if(settings_is_correct_signature()) {
    settings_load(&settings);
  } 
  else { // if the EEPROM data is not valid config data then write config data to EEPROM
    Serial.println("bad signature. Use default settings.");
    settings_reset(&settings);
  }

  // print inhoud
  Serial.print("wifi_sidd = ");
  Serial.println(settings.wifi_sidd);
  Serial.print("wifi_password = ");
  Serial.println(settings.wifi_password);

  // start wifi hotspot
  setup_wifi(settings.mode_is_hotspot, (char *) settings.wifi_sidd,(char *) settings.wifi_password);

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
  Serial.println("Starting Transmitter");

  // this init will set the pinModes for you
  Transceiver.init();
}

void loop() {
  DNS_server.processNextRequest(); 

  static char post_payload[1+POST_PAYLOAD_LEN]; //will be filled with transmitter ID, temperature data, separated with '\n';
  interface_server.handleClient();
  delay(2);

  //THESE LINES HAVE TO BE IMPLEMENTED
  // if the transceiver serial is available, proces incoming data
  // you can also use Transceiver.available()
  if (Transceiver.available()) {
    if(LoRa_get_data()) {
      generate_http_post(&post_payload[0]);
      send_to_cloud(&post_payload[0]);
    }
  }

  // reset settings when serial read a 'r'
  if(Serial.available()) {
    if(Serial.read() == 'r') {
      Serial.println("Default hotspot mode.");
      settings_reset(&settings);
    }
  }
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
  int data_length = (strlen(settings.cloud_address) + (strlen(settings.cloud_port)+1/*:*/) + strlen(settings.cloud_path));
  char server_url[1+data_length];
  bool retval = false;

  if (!WiFi.isConnected()) {
    Serial.println("Error in WiFi connection");
    return retval;
  }

  Serial.print("Data length: ");
  Serial.println(data_length);
  Serial.print("Sending to: ");
  strcpy(server_url, settings.cloud_address);
  strcat(server_url, ":");
  strcat(server_url, settings.cloud_port);
  strcat(server_url, settings.cloud_path);
  Serial.write((uint8_t*) &server_url, sizeof(server_url));
  Serial.println();
  Serial.print("Payload: ");
  Serial.println(payload);

  http.begin(server_url);  //Specify destination for HTTP request
  http.addHeader("Content-Type", "text/plain");  //Specify content-type header
  int httpResponseCode = http.POST((uint8_t*) payload, strlen((const char*)payload)); //Send the actual POST request

  if (httpResponseCode>0) {
    String response = http.getString(); //Get the response to the request
  
    Serial.println(httpResponseCode);   //Print return code
    retval = true;
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }

  http.end(); //Free resources
  return retval;
}

//get the data from the LoRa module via USART
//https://github.com/KrisKasprzak/EBYTE
bool LoRa_get_data() {
  bool result;
  // i highly suggest you send data using structures and not
    // a parsed data--i've always had a hard time getting reliable data using
    // a parsing method
  if(Transceiver.get_struct(&Temperatures, sizeof(Temperatures))) { //if data is the right size, it's more certain the data is from this system.
    result = true;
    for(int i = 0; i < TRANSMITTER_ID_SIZE; i++) {  //if transmitter ID is valid, thing can be sent. Otherwise it's not valid and can cause problems in the cloud
      if(Temperatures.transmitter_ID[i] < 33 || Temperatures.transmitter_ID[i] > 126) { 
        result = false;
        return result;
      }
    }
  } else {
    result = false;
  }
  
  return result;
}
