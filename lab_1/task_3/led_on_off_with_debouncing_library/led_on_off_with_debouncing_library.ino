/*
 * led_on_off_with_debouncing_library.ino
 *
 * latch an led based on the press of a button (now with
 * added button debouncing)
 *
 * using the "Button" library from https://github.com/t3db0t/Button
 * for simplicity
 *
 * author: alex shenfield
 * date:   06/09/2018
 */

// include the button library for ease of debouncing
#include <Button.h>

// set pin numbers for button and led
const int button_pin = 12;
const int led_pin = 13;

// variable for latching the led
int led_state = 1;

// set up the button object (in debounce mode)
Button button = Button(button_pin, BUTTON_PULLUP_INTERNAL, true, 50);

// "button pressed" callback
void on_press(Button& b)
{
  // toggle the led
  digitalWrite(led_pin, led_state);
  led_state = !led_state;
}

// CODE

// set up code
void setup()
{
  // set pin i/o
  pinMode(led_pin, OUTPUT);
  pinMode(button_pin, INPUT);

  // set up button callback for the button pressed event
  button.pressHandler(on_press);
}

// main program
void loop()
{
  // update the state of the button object
  button.process();
}
