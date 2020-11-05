/**
 * debouncing.ino
 * 
 * debouncing code that doesn't rely on delays.
 * 
 * author:  alex shenfield
 * date:    03/11/2020
 */

// set some debounce variables for our button
int curr_button_state = LOW;
int last_button_state = LOW;

unsigned long last_debounce_time = 0;
unsigned long debounce_delay = 50;

int led_state = LOW;

// set up code
void setup() 
{  
  // declare appropriate pins
  pinMode(2, INPUT);
  pinMode(3, OUTPUT);
}

// main program loop
void loop() 
{
  check_button();
  digitalWrite(3, led_state);
}

// check button function
void check_button()
{
  // get the state of the button
  int reading = digitalRead(2);

  // if the button state changed then reset the deboucing timer
  if (reading != last_button_state)
  {
    last_debounce_time = millis();
  }

  // if the reading has been there longer than the debounce delay,
  // then it is a valid state
  if ((millis() - last_debounce_time) > debounce_delay)
  {        
    // so if the button state has changed 
    if (reading != curr_button_state)
    {      
      // then update it
      curr_button_state = reading;

      // if the button has been pressed then toggle the led
      if (curr_button_state == HIGH)
      {
        led_state = !led_state;
      }
    }
  }

  // update last button state
  last_button_state = reading;
}
