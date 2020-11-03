/*
 * adafruit_io_data_post.ino
 * 
 * simple sketch to post numeric data to adafruit.io to demonstrate the use
 * of restful webservices
 * 
 * author: alex shenfield
 * date:   28/09/2020
 */

// INCLUDES

// we are using the SparkFun ESP8266 WiFi shield - but the SparkFun libraries suck
// so we are using the WiFiEspAt library
#include <SoftwareSerial.h> 
#include <WiFiEspAT.h>

#include <RestClient.h>

// my wifi credentials are included as a seperate header file
#include "MyCredentials.h"

// WIFI INITIALISATION

// create our software serial connection to the esp8266
SoftwareSerial esp8266(8, 9);
WiFiClient wifi_client;

// set the rest connection information
RestClient restclient = RestClient("io.adafruit.com");

// CODE

// set up code
void setup()
{
  // set up serial comms for debugging and display of DHCP allocated ip
  // address
  Serial.begin(9600);
  Serial.println("starting rest web service on arduino ...");

  // set up the esp8266 module
  esp8266.begin(9600);
  if (!WiFi.init(esp8266))
  {
    Serial.println("error talking to ESP8266 module");
    while(true)
    {
    }
  }
  Serial.println("ESP8266 connected");

  // connect to wifi
  WiFi.begin(mySSID, myPSK);

  // waiting for connection to Wifi network
  Serial.println("waiting for connection to WiFi");
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print('.');
  }
  Serial.println();
  Serial.println("connected to WiFi network");  

  // print the IP address to the serial monitor
  IPAddress myIP = WiFi.localIP();
  Serial.print("My IP: "); 
  Serial.println(myIP);
}

// main program
void loop()
{
  // initialise a blank response string
  String response = "";

  // assemble the http headers
  restclient.setHeader(xaiokey);
  restclient.setHeader("Content-Type: application/json");

  // read the data and create the json string
  char data[64];
  int reading = analogRead(A0);
  sprintf(data, "{\"value\":\"%u\"}", reading);

  Serial.println(data);

  // post the data
  int statusCode = restclient.post("/api/feeds/testing.numbers/data", data, &response);

  // report on the outcome
  Serial.print("status code from server: ");
  Serial.println(statusCode);
  Serial.print("response body from server: ");
  Serial.print(response);
  
  // delay a little
  delay(5000);
}
