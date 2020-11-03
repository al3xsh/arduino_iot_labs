/*
 * adafruit_io_rest_data_logger.ino
 * 
 * this sketch talks to the adafruit io service via rest web service calls
 * and uses the RestClient library to ensure properly formatted requests
 * 
 * https://github.com/csquared/arduino-restclient
 * 
 * note: it is / should be possible to use the standard ethernet client to
 * do this - but my experience is that creating properly formatted http 
 * requests by hand is *hard*!
 *  
 * author:  alex shenfield
 * date:    08/10/2016
 */

// INCLUDES

// note: the ethernet shields we have here at SHU use the Ethernet2.h 
// library
//#include <Ethernet.h>
#include <Ethernet2.h>
#include <SPI.h>
#include <RestClient.h>

// ETHERNET DECLARATIONS

// the ethernet shields and ethernet libraries support DHCP as long as
// you are plugged into a device with a DHCP server however, if you
// are plugged straight in to the network switch in the cabinet (as
// in the embedded lab) you will need to allocate a static ip address
IPAddress ip(192, 168, 1, 200);
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);

// ethernet shield mac address
byte mac[] = {0x90, 0xA2, 0xDA, 0x00, 0x5B, 0x13};

// REST CLIENT STUFF

// define the rest client
RestClient restclient = RestClient("io.adafruit.com");

void setup() 
{
  // set up serial comms for debugging and display of DHCP allocated ip 
  // address
  Serial.begin(115200);
  Serial.println("starting rest web service poster on arduino ...");

  // set pin modes
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);

  // start the ethernet shield comms - initially try to get a DHCP ip 
  // address
  if (Ethernet.begin(mac) == 0) 
  {
    // if DHCP fails, allocate a static ip address
    Serial.println("failed to configure ethernet using DHCP");
    Ethernet.begin(mac, ip);
  }

  // print the IP address to the serial monitor
  Serial.print("web service client is at: ");
  Serial.println(Ethernet.localIP());

  // just wait a little for everything to be set up
  delay(1000);
  Serial.println("setup completed!");
}

void loop() 
{
  // send a status message ...
  send_status();
  delay(100);

  // read and transmit the light levels (ldr connected to A0) ...
  send_light();
  delay(100);
    
  // read and transmit the temperature levels (tmp36 connected to A1) ...
  send_temp();  

  // delay a little
  delay(10000);
}

// METHODS

// send the status message
void send_status()
{
  // initialise a blank response (to be filled in later)
  String response = "";
  
  // assemble the http headers
  restclient.setHeader("x-aio-key: 47fcedafc7b648d5baa7d3f8feabc822");
  restclient.setHeader("Content-Type: application/json");

  // send a status message ...

  // post the http request
  char status_msg[] = "{\"value\":\"connected :)\"}";
  int statuscode = restclient.post("/api/feeds/status/data", status_msg, &response);
  
  // report on the outcome
  Serial.println("status code from server: ");
  Serial.println(statuscode);
  Serial.println("response body from server: ");
  Serial.println(response);  
}

// send the light levels
void send_light()
{
  // initialise a blank response (to be filled in later)
  String response = "";
  
  // assemble the http headers
  restclient.setHeader("x-aio-key: 47fcedafc7b648d5baa7d3f8feabc822");
  restclient.setHeader("Content-Type: application/json");

  // read the data and create the data string (dividing by 30 so the scale approximates to
  // the temperature reading)
  char ldr_data[64];
  int ldr_reading = analogRead(A0) / 30;
  sprintf(ldr_data, "{\"value\":\"%u\"}", ldr_reading);
  
  // post the http request
  int statuscode = restclient.post("/api/feeds/light/data", ldr_data, &response);
  
  // report on the outcome
  Serial.println("status code from server: ");
  Serial.println(statuscode);
  Serial.println("response body from server: ");
  Serial.println(response);    
}

// send the temperature
void send_temp()
{
  // initialise a blank response (to be filled in later)
  String response = "";
  
  // assemble the http headers
  restclient.setHeader("x-aio-key: 47fcedafc7b648d5baa7d3f8feabc822");
  restclient.setHeader("Content-Type: application/json");

  // read the data and convert to temperature in C
  int tmp_reading = analogRead(A1);
  float tmp = (((5000.0 * tmp_reading) / 1023.0) - 500.0) / 10.0;
  
  // create a string for temperature data

  // arduino sprintf doesn't implement floats, so we will fiurst use dtostrf to 
  // convert float to string ...  
  char tmp_str[10];
  dtostrf(tmp, 5, 2, tmp_str);

  // ... then use that string to create our json!
  char tmp_data[64];
  sprintf(tmp_data, "{\"value\":\"%s\"}", tmp_str);
  
  // post the http request
  int statuscode = restclient.post("/api/feeds/temperature/data", tmp_data, &response);
  
  // report on the outcome
  Serial.println("status code from server: ");
  Serial.println(statuscode);
  Serial.println("response body from server: ");
  Serial.println(response);
}
