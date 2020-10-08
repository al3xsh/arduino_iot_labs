/*
 * restful_arduino_from_scratch.ino
 *
 * simple rest based webservice with extra functionality
 *
 * author:  alex shenfield
 * date:    10/09/2020
 */

// we are using the SparkFun ESP8266 WiFi shield - but the SparkFun libraries suck
// so we are using the WiFiEspAt library
#include <SoftwareSerial.h> 
#include <WiFiEspAT.h>

// my wifi credentials are included as a seperate header file
#include "MyCredentials.h"

// create our software serial connection to the esp8266
SoftwareSerial esp8266(8, 9);

// set up a server on port 80 (note: web browsers usually
// assume that the server is running on port 80 unless told
// otherwise)
WiFiServer server(80);

// variable indicating when to start paying attention to data
boolean pay_attention = true;

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

  // set up the pins as outputs
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
}

// main code
void loop()
{
  // constantly check for connections
  check_for_connections();
}

// method to check for incoming connections and process them
void check_for_connections()
{
  // get any client that is connected to the server and
  // trying to send data
  WiFiClient client = server.available();

  // record whether we have already sent the standard http
  // response header
  boolean header_sent = false;

  // if a remote client is connected
  if (client)
  {
    // get the ip address of the remote client
    IPAddress ip = client.remoteIP();
    Serial.print("new client at ");
    Serial.println(ip);
    
    // while the client is connected ...
    while (client)
    {
      // if we haven't already sent the http header
      if (!header_sent)
      {
        // send standard http response header (to acknowledge the
        // data)
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println();
        header_sent = true;
      }
  
      // ... and has more data to send
      if (client.available() > 0)
      {
        // read the next byte
        char c = client.read();
  
        // debugging
        Serial.print(c);
  
        // pay attention to all the data between the '?' character
        // and a space
        if (c == '?')
        {
          pay_attention = true;
        }
        else if (c == ' ')
        {
          pay_attention = false;
        }
  
        // if we are paying attention ...
        if (pay_attention)
        {
          // use a switch statement to decide what to do
          switch (c)
          {
            case '2':
              trigger_pin(2, client);
              break;
  
            case '3':
              trigger_pin(3, client);
              break;
  
            case '4':
              trigger_pin(4, client);
              break;
  
            case '5':
              trigger_pin(5, client);
              break;
  
            case 'x':
              clear_pins(client);
              break;
  
            case 'p':
              read_potentiometer(client);
              break;
          }
        }
      }
      // when the client is done sending data
      else
      {
        // disconnect from client
        client.stop();
      }
    }
  }
}

// separate function for triggering pins - note we only need
// the ethernet client so we can send data to it
void trigger_pin(int pin_number, WiFiClient client)
{
  // print a status message
  client.print("Turning on pin <b>");
  client.println(pin_number);
  client.println("</b><br>");

  // turn on appropriate pin
  digitalWrite(pin_number, HIGH);
}

// another function to clear all pins - again the ethernet client
// is only needed to return data to the web page
void clear_pins(WiFiClient client)
{
  // print a status message
  client.println("Clearing all pins!<br>");

  // turn off pins
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
}

// another function to read the potentiometer - again the ethernet client
// is only needed to return data to the web page
void read_potentiometer(WiFiClient client)
{
  // print a status message
  client.println("Reading potentiometer!<br>");

  // read potentiometer and display adc value
  client.print("Potentiometer value = <b>");
  client.println(analogRead(A0));
  client.println("</b><br>");  
}
