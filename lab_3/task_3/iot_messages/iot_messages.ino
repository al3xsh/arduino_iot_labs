/*
 * iot_messages.ino
 * 
 * a simple mqtt application to parse mqtt messages on subscribed 
 * topics
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
byte mac[] = {0x90, 0xA2, 0xDA, 0x0F, 0xF5, 0xE7};

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

    // do nothing else here (as we are just testing how message
    // subscriptions work)
    // ...
  }
}

// MQTT FUNCTIONS

// mqtt message received callback
void callback(char* topic, byte* payload, unsigned int length) 
{  
  // get the topic
  String t = String(topic);
  
  // get the value of the message
  char data[length + 1];
  for (int i = 0; i < length; i++)
  {
    data[i] = payload[i];
  }
  data[length] = '\0';
  
  // print the message to the serial window
  Serial.print("message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(data);
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

      // once connected, publish an announcement ...
      client.publish("alexshenfield/f/status-messages", "we are alive!");
      
      // ... and subscribe to the appropriate feeds (including the 
      // "throttle" topic which will let us know if we are bumping up 
      // against the adafruit.io rate limit)
      client.subscribe("alexshenfield/f/status-messages");
      client.subscribe("alexshenfield/f/light-switch");
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

