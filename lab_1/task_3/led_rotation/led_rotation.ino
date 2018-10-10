/*
 * led_rotation.ino
 *
 * cycle through leds based on the press of a button (now with
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

// set pin numbers for button and leds (also set the number of leds
// to cycle through)
const int button_pin = 2;

const int n_leds = 3;
const int led_pin_1 = 13;
const int led_pin_2 = 12;
const int led_pin_3 = 11;

// variable for controlling the leds
int led_state = 1;

// set up the button object (in debounce mode)
Button button = Button(button_pin, BUTTON_PULLUP_INTERNAL, true, 50);

// "button pressed" callback
void on_press(Button& b)
{
  Serial.println("button press!");
  led_state = (led_state + 1) % n_leds;
}

// CODE

// set up code
void setup()
{
  // set up serial for debugging
  Serial.begin(9600);
  Serial.println("we are alive!");
  
  // set pin i/o
  pinMode(led_pin_1, OUTPUT);
  pinMode(led_pin_2, OUTPUT);
  pinMode(led_pin_3, OUTPUT);
  pinMode(button_pin, INPUT);

  // set up button callback for the button pressed event
  button.pressHandler(on_press);
}

// main program
void loop()
{
  // update the state of the button object
  button.process();

  // use a switch - case statement to control which leds 
  // to trigger
  switch(led_state)
  {
    case 0:
      digitalWrite(led_pin_1, HIGH);
      digitalWrite(led_pin_2, LOW);
      digitalWrite(led_pin_3, LOW);
      break;
    case 1:
      digitalWrite(led_pin_1, LOW);
      digitalWrite(led_pin_2, HIGH);
      digitalWrite(led_pin_3, LOW);
      break;
    case 2:
      digitalWrite(led_pin_1, LOW);
      digitalWrite(led_pin_2, LOW);
      digitalWrite(led_pin_3, HIGH);
      break;
    // we shouldn't ever get here - but just in case ...
    default:
      digitalWrite(led_pin_1, LOW);
      digitalWrite(led_pin_2, LOW);
      digitalWrite(led_pin_3, LOW);
      break;      
  }
}
