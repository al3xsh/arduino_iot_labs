/*
 * restful_arduino.ino
 *
 * arduino restful api example using the aREST library - in this example we are
 * setting the brightness of an led on pin 3 and reading light values
 *
 * author:  alex shenfield
 * date:    10/09/2018
 */

// INCLUDES

// note: the ethernet shields we have here at SHU use the Ethernet2.h
// library
#include <Ethernet2.h>
#include <SPI.h>

// include the aREST library
#include <aREST.h>

// include the avr watchdog timer library
#include <avr/wdt.h>

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

// AREST DECLARATIONS

// create an aREST instance
aREST rest = aREST();

// variables to monitor with our webservice
int light_level = 0;

// CODE

// set up code
void setup()
{
  // set up serial comms for debugging and display of DHCP allocated ip
  // address
  Serial.begin(9600);
  Serial.println("starting rest web service on arduino ...");

  // set pin modes
  pinMode(3, OUTPUT);
  pinMode(A0, INPUT);

  // expose the light level variable to the REST api
  rest.variable("light_level", &light_level);

  // expose the led trigger function to the REST api
  rest.function("led", led_control);

  // set the name and id for this webservice node
  rest.set_id("001");
  rest.set_name("alexs_arduino");

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
  Serial.print("web service is at: ");
  Serial.println(Ethernet.localIP());

  // start watchdog timer
  wdt_enable(WDTO_4S);
}

// main loop
void loop()
{
  // update our light level each time round the loop
  light_level = analogRead(A0);

  // listen for incoming clients
  EthernetClient client = server.available();
  rest.handle(client);
  wdt_reset();
}

// FUNCTIONS EXPOSED TO THE REST API

// led control function accessible by the API
int led_control(String command)
{
  // debugging information about the actual command string
  Serial.print("command is ");
  Serial.println(command);

  // get pwm signal from command
  int pwm = command.toInt();
  pwm = constrain(pwm, 0, 255);

  // send pwm signal to the led
  analogWrite(3, pwm);

  // return 1 (indicating success)
  return 1;
}
