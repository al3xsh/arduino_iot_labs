/*
 * adafruit_io_data_post.ino
 * 
 * simple sketch to post numeric data to adafruit.io to demonstrate the use
 * of restful webservices
 * 
 * author: alex shenfield
 * date:   08/10/2018
 */

#include <Ethernet2.h>
#include <SPI.h>
#include "RestClient.h"

// set the rest connection information
RestClient restclient = RestClient("io.adafruit.com");

// ETHERNET DECLARATIONS

// set up a server on port 80 to host our webservice
EthernetServer server = EthernetServer(80);

// the ethernet shields and ethernet libraries support DHCP as long as
// you are plugged into a device with a DHCP server however, if you
// are plugged straight in to the network switch in the cabinet (as
// in the embedded lab) you will need to allocate a static ip address
IPAddress ip(192, 168, 137, 11);
IPAddress gateway(192, 168, 137, 254);
IPAddress subnet(255, 255, 255, 0);

// ethernet shield mac address
byte mac[] = {0x90, 0xA2, 0xDA, 0x0F, 0xF5, 0xAD};

// CODE

// CODE

// set up code
void setup()
{
  // set up serial comms for debugging and display of DHCP allocated ip
  // address
  Serial.begin(9600);
  Serial.println("starting rest client on arduino ...");

  // set an analog input pin
  pinMode(A0, INPUT);

  // start the ethernet shield comms - initially try to get a DHCP ip
  // address
  if (Ethernet.begin(mac) == 0)
  {
    // if DHCP fails, allocate a static ip address
    Serial.println("failed to configure ethernet using DHCP");
    Ethernet.begin(mac, ip);
  }

  // start the server, and print the IP address to the serial
  // monitor
  server.begin();
  Serial.print("rest client is at: ");
  Serial.println(Ethernet.localIP());

  // wait for everything to settle down
  delay(1000);
  Serial.println("setup complete!");

}

// main program
void loop()
{
  // initialise a blank response string
  String response = "";

  // assemble the http headers
  restclient.setHeader("x-aio-key: 47fcedafc7b648d5baa7d3f8feabc822");
  restclient.setHeader("Content-Type: application/json");

  // read the data and create the json string
  char data[64];
  int reading = analogRead(A0);
  sprintf(data, "{\"value\":\"%u\"}", reading);

  // post the data
  int statusCode = restclient.post("/api/feeds/testing.numbers/data", data, &response);

  // report on the outcome
  Serial.print("status code from server: ");
  Serial.println(statusCode);
  Serial.print("response body from server: ");
  Serial.println(response);
  
  // delay a little
  delay(5000);
}
