/*
 * restful_arduino.ino
 *
 * arduino restful api example using the aREST library - in this example we are
 * setting the brightness of an led on pin 3 and reading light values
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

// include the aREST library (making sure that it knows we are using a WiFi client
#define WiFi_h 1
#include <aREST.h>

// include the avr watchdog timer library
#include <avr/wdt.h>

// WIFI INITIALISATION

// create our software serial connection to the esp8266
SoftwareSerial esp8266(8, 9);

// set up a server on port 80 (note: web browsers usually
// assume that the server is running on port 80 unless told
// otherwise)
WiFiServer server(80);

// AREST DECLARATIONS

// create an aREST instance
aREST rest = aREST();

// variables to monitor with our webservice
int light_level = 0;
int temp_level = 0;

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
  
  // start the server
  server.begin();

  // set pin modes
  pinMode(2, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);

  // expose the temperature and light level variables to the REST api
  rest.variable("light_level", &light_level);
  rest.variable("temp_level", &temp_level);

  // expose the led trigger function to the REST api
  rest.function("led", led_control);

  // set the name and id for this webservice node
  rest.set_id("001");
  rest.set_name("alexs_arduino");

  // start watchdog timer
  wdt_enable(WDTO_4S);
}

// main loop
void loop()
{
  // update our temperature and light levels each time round the loop
  temp_level = analogRead(A0);
  light_level = analogRead(A1);

  // listen for incoming clients
  WiFiClient client = server.available();
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
  analogWrite(2, pwm);

  // return 1 (indicating success)
  return 1;
}
