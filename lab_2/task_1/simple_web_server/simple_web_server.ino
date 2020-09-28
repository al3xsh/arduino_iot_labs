/*
 * simple_web_server.ino
 *
 * a simple web server for demonstrating arduino and wifi
 * connectivity
 *
 * author:  alex shenfield
 * date:    10/09/2020
 */

// INCLUDES

// we are using the SparkFun ESP8266 WiFi shield - but the SparkFun libraries suck
// so we are using the WiFiEspAt library
#include <SoftwareSerial.h> 
#include <WiFiEspAT.h>

// my wifi credentials are included as a seperate header file
#include "MyCredentials.h"

// WIFI INITIALISATION

// create our software serial connection to the esp8266
SoftwareSerial esp8266(8, 9);

// set up a server on port 80 (note: web browsers usually
// assume that the server is running on port 80 unless told
// otherwise)
WiFiServer server(80);

// CODE

// set up code
void setup()
{
  // set up serial comms for debugging and display of DHCP allocated IP address
  Serial.begin(9600);

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
  
  // start the server
  server.begin();
}

// main code
void loop()
{
  // check for a client connection
  WiFiClient client = server.available();

  // if a remote client is connected
  if (client)
  {
    // get the ip address of the remote client
    IPAddress ip = client.remoteIP();
    Serial.print("new client at ");
    Serial.println(ip);

    // while the client is still connected
    while (client)
    {
      // and has more data to send
      if (client.available() > 0)
      {
        // read bytes from the incoming client and write them to
        // the serial monitor
        Serial.print((char)client.read());
      }
      // when the client is done sending data
      else
      {
        // send standard http response header (to acknowledge the
        // data)
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println();
  
        // disconnect from client
        client.stop();
      }
    }
  }
}
