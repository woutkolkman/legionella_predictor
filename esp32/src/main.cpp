
#include "main.h"

#if TRANSCEIVE == 0

void setup() {


  Serial.begin(115200);

  Serial2.begin(9600);
  Serial.println("Starting Reader");

  // this init will set the pinModes for you
  Transceiver.init();

}

void loop() {

  // if the transceiver serial is available, proces incoming data
  // you can also use Transceiver.available()

  if (Transceiver.available()) {

    // i highly suggest you send data using structures and not
    // a parsed data--i've always had a hard time getting reliable data using
    // a parsing method

    Transceiver.GetStruct(&MyData, sizeof(MyData));

    // dump out what was just received
    Serial.print("Count: "); Serial.println(MyData.Count);
    Serial.print("Temp: "); Serial.println(MyData.Temperature);

    // if you got data, update the checker
    Last = millis();

  }
  else {
    // if the time checker is over some prescribed amount
    // let the user know there is no incoming data
    if ((millis() - Last) > 1000) {
      Serial.println("Searching: ");
      Last = millis();
    }

  }
}
#elif TRANSCEIVE == 1

void setup() {

  MyData.Temperature = 0;
  up = true;
  Serial.begin(115200);
  Serial2.begin(9600);

  Serial.println("Starting Reader");

  // this init will set the pinModes for you
  Serial.println(Transceiver.init());

}

void loop() {

  // measure some data and save to the structure
  if(up) {
    MyData.Temperature++;
  } else {
    MyData.Temperature--;
  }
  MyData.Count++;
  if(MyData.Temperature <= -10) {
    up = true;
  }
  if(MyData.Temperature >= 85) {
    up = false;
  }
  
  //Sending data as struct
  Transceiver.SendStruct(&MyData, sizeof(MyData));

  // let the use know something was sent
  Serial.print("Sending Count: ");Serial.print(MyData.Count);Serial.print(" = Temp: "); Serial.println(MyData.Temperature);
  delay(1000);

}
#endif
