/*
 * mqtt_message_test.ino
 *
 * basic mqtt sketch to send a simple status message to an adafruit.io
 * feed and then subscribe to all the feeds. it then periodically sends
 * status update messages (once every 10 seconds) containing the current
 * uptime of the system.
 *
 * author:  alex shenfield
 * date:    10/09/2018
 */

// include necessary libraries
#include <SPI.h>
#include <Ethernet2.h>
#include <PubSubClient.h>

// ETHERNET CONNECTION DECLARATIONS

// mac address
byte mac[] = {0x90, 0xA2, 0xDA, 0x11, 0x44, 0xC7};

// the ethernet shields and ethernet libraries support DHCP as long as
// you are plugged into a device with a DHCP server - it is always a
// good idea to have a static address to fall back on though!
IPAddress ip(192, 168, 0, 11);
IPAddress gateway(192, 168, 0, 254);
IPAddress subnet(255, 255, 255, 0);

// MQTT DECLARATIONS

// mqtt server details
const char userid[] = "alexshenfield";
const char apikey[] = "fcd8f4ce6cd14d8ba860ebb50fc312fa";
const char server[] = "io.adafruit.com";
const long port     = 1883;

// get an ethernet client and pass it to the mqtt client
EthernetClient ethernet_client;
PubSubClient client(ethernet_client);

// timing variables - note: adafruit.io allows a maximum of 30 messages
// per minute - any more than this and your account will be throttled
// and then blocked!
long previous_time = 0;
long connection_interval = 10000;

// CODE

// set up code
void setup()
{
  // set up serial comms for debugging and display of DHCP allocated ip
  // address
  Serial.begin(9600);
  Serial.println("starting mqtt client on arduino ...");

  // mqtt server and subscription callback
  client.setServer(server, port);
  client.setCallback(callback);

  // start the ethernet shield comms - initially try to get a DHCP ip
  // address
  if (Ethernet.begin(mac) == 0)
  {
    // if DHCP fails, allocate a static ip address
    Serial.println("failed to configure ethernet using DHCP");
    Ethernet.begin(mac, ip);
  }

  // allow the hardware to sort itself out
  delay(1500);

  // print the ip address to the serial monitor
  Serial.print("mqtt client is at: ");
  Serial.println(Ethernet.localIP());
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
    if (client.connect("my_arduino", userid, apikey))
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
