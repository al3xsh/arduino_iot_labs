/*
 * html_web_server.ino
 * 
 * a simple web server for demonstrating arduino and ethernet
 * connectivity by serving a simple html page
 * 
 * author:  alex shenfield
 * date:    10/09/2018
 */

// note: the ethernet shields we have here at SHU use the
// Ethernet2.h library - however, many of the other shields on
// the market use the original Ethernet.h library!
#include <Ethernet2.h>
#include <SPI.h>

// set up a server on port 80 (note: web browsers usually
// assume that the server is running on port 80 unless told
// otherwise)
EthernetServer server = EthernetServer(80);

// the ethernet shields and ethernet libraries support DHCP as long as
// you are plugged into a device with a DHCP server however, if you
// are plugged straight in to the network switch in the cabinet (as
// in the embedded lab) you will need to allocate a static ip address
IPAddress ip(192, 168, 137, 11);
IPAddress gateway(192, 168, 137, 254);
IPAddress subnet(255, 255, 255, 0);

// ethernet shield mac address (i.e. the hexadecimal numbers found
// on the bottom of the shield)
byte mac[] = {0x90, 0xA2, 0xDA, 0x0F, 0xF5, 0xAD};

// CODE

// set up code
void setup()
{
  // set up serial comms for debugging and display of
  // DHCP allocated IP address
  Serial.begin(9600);

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
  Serial.println(Ethernet.localIP());
}

// main code
void loop()
{
  // check for a client connection
  EthernetClient client = server.available();

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

      // send web page
      client.println("<html>");
      client.println("<body style=background-color:#FF0000>");
      client.println("<font color=#FFFFFF>");
      client.println("<h1>This is my example web page</h1>");
      client.println("<hr>");
      client.println("<p>It has some text ...</p>");
      client.println("<a href=\"http://lmgtfy.com/?q=arduino+html\">"
                     "and a link to all the information you need</a>");
      client.println("</body>");
      client.println("</html>");

      // disconnect from client
      client.stop();
    }
  }
}
