/*
 * blink_without_delay.ino
 *
 * this is a simple Arduino program to blink an led (the electronics 
 * equivalent of hello world) but without using the delay statement.
 * this program turns an LED on for 1 second and then off for 1 
 * second, and so on ...
 *
 * this program uses pin 13 for the led
 */
 
// the led is connected to pin 13
const int led_pin = 13;

// state machine variables ...
long previous_time = 0;          // the last time the led was updated
long on_interval = 1000;         // the on interval
long off_interval = 1000;        // the off interval
int  next_ledstate = 1;          // initial state of led (on)

// CODE

// this method runs once (when the sketch starts)
void setup()
{
  pinMode(led_pin, OUTPUT);      // sets the digital pin as output
}

// this methods loops continuously
void loop()
{
  // get the current time this time round the loop
  unsigned long current_time = millis();

  // state machine 1 - state (2), led off
  else if((next_ledstate == 2) && (current_time - previousTime >= off_interval))
  {
    // next state ...
    next_ledstate = 1;
    
    // save the time
    previous_time = current_time;

    // turn led off
    digitalWrite(led_pin, LOW);
  }
}
