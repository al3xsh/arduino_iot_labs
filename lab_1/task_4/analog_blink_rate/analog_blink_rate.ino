/*
 * analog_blink_rate.ino
 *
 * simple program to adjust the blink rate of an led based
 * upon the value of a potentiometer
 */

// set pin numbers
const int led_pin = 11;
const int pot_pin = A0;

// initialise the state of the led
int led_state = LOW;
 
// CODE

// set up code
void setup() 
{
  // set up the serial connection to enable debugging
  Serial.begin(9600);
  
  // set the pin i/o
  pinMode(led_pin, OUTPUT);
}

// main program
void loop() 
{
  // get the value of the pot
  int pot_val = analogRead(pot_pin);
  
  // debugging
  Serial.print("Potentiometer value: ");
  Serial.println(pot_val, DEC);
  
  // flip the state of the LED
  if(led_state == HIGH)
  {
    led_state = LOW;
  }
  else
  {
    led_state = HIGH;
  }
  
  // then write it out and wait for the delay set by the pot
  digitalWrite(led_pin, led_state);
  delay(pot_val);
  
}
 
