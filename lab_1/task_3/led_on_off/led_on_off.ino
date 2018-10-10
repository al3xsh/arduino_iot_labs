/*
 * led_on_off.ino
 *
 * simple sketch to turn an led on when the pushbutton is held
 * down and turn it off when it is released
 *
 */

// set pin numbers for button and led
const int button_pin = 12;
const int led_pin =  13;

// CODE

// set up code
void setup()
{
  // set pin i/o
  pinMode(led_pin, OUTPUT);
  pinMode(button_pin, INPUT);
}

// main program
void loop()
{
  // read the state of the pushbutton value:
  int button_state = digitalRead(button_pin);

  // check if the pushbutton is pressed.
  // if it is turn the led on
  if (button_state == HIGH)
  {
    digitalWrite(led_pin, HIGH);
  }
  // if it isn't turn led off
  else
  {
    digitalWrite(led_pin, LOW);
  }
}
