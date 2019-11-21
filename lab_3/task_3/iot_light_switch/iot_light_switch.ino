/*
 * iot_light_switch.ino
 * 
 * an internet of things light switch that integrates with adafruit io
 * to allow remote control of lighting
 * 
 * WARNING: if you toggle the light switch too fast, you are likely to
 * hit the rate limits of adafruit.io - pay attention to the throttle
 * topic in the serial window
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

// DEBOUNCING AND LED DECLARATIONS

// light bulb and light switch pins
const int light_bulb = 3;
const int light_switch = 2;

// set some debounce variables for our button
int curr_button_state = LOW;
int last_button_state = LOW;

unsigned long last_debounce_time = 0;
unsigned long debounce_delay = 50; 

int led_state = LOW;

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
    // handle the light switch
    update_light_state();
    digitalWrite(light_bulb, led_state);
  
    // handle mqtt functions (e.g. subscriptions to topics)
    client.loop();
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

  // parse the light switch topic to work out what the light bulb
  // should be doing
  if (t.indexOf("light-switch") > 0)
  {
    if (strcmp(data, "ON") == 0)
    {
      led_state = HIGH;
    }
    else if (strcmp(data, "OFF") == 0)
    {
      led_state = LOW;
    }
  }
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

      // also publish the light switch state (so that we reset any UI controls) ...
      send_light_status(led_state);
      
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

// APPLICATION FUNCTIONS

// send the light switch status to the mqtt broker
void send_light_status(int led)
{
  // if the client is already connected then send data
  if (client.connected())
  {    
    // send "ON" if the led is on, and "OFF" if the led is off
    if (led == HIGH)
    {
      client.publish("alexshenfield/f/light-switch", "ON");
    }
    else
    {
      client.publish("alexshenfield/f/light-switch", "OFF");      
    }
  }
}

// update the state of the light by reading the light switch and debouncng it
void update_light_state()
{
  // get the state of the light switch
  int reading = digitalRead(light_switch);

  // if the light switch state has changed then reset the deboucing 
  // timer
  if (reading != last_button_state)
  {
    last_debounce_time = millis();
  }

  // if the reading has been there longer than the debounce delay,
  // then it is a valid state
  if ((millis() - last_debounce_time) > debounce_delay)
  {        
    // so if the light switch state has changed 
    if (reading != curr_button_state)
    {      
      // then update it
      curr_button_state = reading;

      // if the button has been pressed then toggle the led state and 
      // update the mqtt broker
      if (curr_button_state == HIGH)
      {
        led_state = !led_state;
        send_light_status(led_state);
      }
    }
  }

  // update last button state
  last_button_state = reading;
}
