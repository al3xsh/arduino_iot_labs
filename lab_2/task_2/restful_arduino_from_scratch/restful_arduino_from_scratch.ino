/*
 * web_addressable_leds.ino
 *
 * simple web service to control LEDs
 *
 * author:  alex shenfield
 * date:    10/09/2018
 */

#include <Ethernet2.h>
#include <SPI.h>

// set up a server on port 80 (note: web browsers usually
// assume that the server is running on port 80 unless told
// otherwise)
EthernetServer server = EthernetServer(80);

// ip address, gateway and network mask for the web service
IPAddress ip(192, 168, 137, 11);
IPAddress gateway(192, 168, 137, 254);
IPAddress subnet(255, 255, 255, 0);

// ethernet shield mac address (i.e. the hexadecimal numbers found
// on the bottom of the shield)
byte mac[] = {0x90, 0xA2, 0xDA, 0x0F, 0xF5, 0xAD};

// variable indicating when to start paying attention to data
boolean pay_attention = true;

// CODE

// set up code
void setup()
{
  // set up serial comms for debugging and display of
  // DHCP allocated IP address
  Serial.begin(9600);

  // set up pins
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  pinMode(A0, INPUT);

  // start the ethernet shield comms - initially try to get a DHCP ip
  // address
  if (Ethernet.begin(mac) == 0)
  {
    // if DHCP fails, allocate a static ip address
    Serial.println("failed to configure ethernet using DHCP");
    Ethernet.begin(mac, ip);
  }

  // start the server, and print the IP address to the serial monitor
  server.begin();
  Serial.println(Ethernet.localIP());
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
  EthernetClient client = server.available();

  // record whether we have already sent the standard http
  // response header
  boolean header_sent = false;

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

// separate function for triggering pins - note we only need
// the ethernet client so we can send data to it
void trigger_pin(int pin_number, EthernetClient client)
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
void clear_pins(EthernetClient client)
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
void read_potentiometer(EthernetClient client)
{
  // print a status message
  client.println("Reading potentiometer!<br>");

  // read potentiometer and display adc value
  client.print("Potentiometer value = <b>");
  client.println(analogRead(A0));
  client.println("</b><br>");  
}
