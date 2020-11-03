/*
 * mqtt_message_test.ino
 *
 * basic mqtt sketch to send a simple status message to an adafruit.io
 * feed and then subscribe to all the feeds. it then periodically sends
 * status update messages (once every 10 seconds) containing the current
 * uptime of the system.
 *
 * author:  alex shenfield
 * date:    03/11/2020
 */

// INCLUDES

// we are using the SparkFun ESP8266 WiFi shield - but the SparkFun 
// libraries suck so we are using the WiFiEspAt library
#include <SoftwareSerial.h> 
#include <WiFiEspAT.h>

/// include the pubsub client
#include <PubSubClient.h>

// my wifi credentials are included as a seperate header file
#include "MyCredentials.h"

// WIFI INITIALISATION

// create our software serial connection to the esp8266
SoftwareSerial esp8266(8, 9);

// MQTT DECLARATIONS

// mqtt server details
const char server[] = "io.adafruit.com";
const long port     = 1883;

// get a wifi client object and pass it to the mqtt client
WiFiClient wifi_client;
PubSubClient client(wifi_client);

// timing variables - note: adafruit.io allows a maximum of 30 messages
// per minute - any more than this and your account will be throttled
// and then blocked!
long previous_time = 0;
long connection_interval = 10000;

// CODE

// set up code
void setup()
{
  // set up serial comms for debugging and display of DHCP allocated IP 
  // address
  Serial.begin(9600);
  Serial.println("starting mqtt client on arduino ...");

  // mqtt server and subscription callback
  client.setServer(server, port);
  client.setCallback(callback);

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

// main loop
void loop()
{  
  // if the client isn't connected then try to reconnect
  if (!client.connected())
  {
    reconnect();
  }
  else
  {
    // handle subscriptions to topics (i.e. incoming messages)
    client.loop();

    // periodically publish a message to a feed (note, this uses the 
    // same non blocking timing mechanism as in blink_without_delay.ino
    // from lab 1)
    unsigned long current_time = millis();
    if(current_time - previous_time > connection_interval)
    {
      previous_time = current_time;
      send_message();
    }
  }
}

// MQTT FUNCTIONS

// mqtt message received callback
void callback(char* topic, byte* payload, unsigned int length)
{
  // print the feed the message comes from
  Serial.print("message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  // print the message
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// reconnect to server
void reconnect()
{
  // loop until we're reconnected
  while (!client.connected())
  {
    Serial.println("attempting mqtt connection...");

    // try to connect to adafruit.io
    if (client.connect("axs_arduino_190310", userid, apikey))
    {
      Serial.println("... connected");

      // once connected, publish an announcement and subscribe to all 
      // feeds (also subscribe to the "throttle" topic which will let us
      // know if we are bumping up against the adafruit.io rate limit)
      client.publish("alexshenfield/f/status-messages", "we are alive!");
      client.subscribe("alexshenfield/f/#");
      client.subscribe("alexshenfield/throttle");
    }
    else
    {
      // print some error status
      Serial.print("connection failed, rc = ");
      Serial.print(client.state());
      Serial.println();
      Serial.println("we will try again in 5 seconds");

      // wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// APPLICATION FUNCTIONS

// convert the current uptime to a string and publish to the status-messages
// feed
void send_message()
{
  char time_str[40];
  sprintf(time_str, "up for %lu s", (millis() / 1000));  
  client.publish("alexshenfield/f/status-messages", time_str);
}
