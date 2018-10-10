/*
* temperature_sensor.ino
*
* simple program to read the temperature from a tmp36 every 5s and
* display on the serial monitor
*/

// set pin numbers
const int tmp_pin = A0;

// state machine variables ...
long previous_time  = 0;
long query_interval = 5000;

// CODE

// set up code
void setup()
{
  // set up the serial connection to enable debugging
  Serial.begin(9600);
}

// main program
void loop()
{
  // get the current time this time round the loop
  unsigned long current_time = millis();
  
  // if it's time to query the temperature sensor ...
  if ((current_time - previous_time) >= query_interval)
  {
    // save the time
    previous_time = current_time;
    
    // get the temperature
    get_temperature();
  }
}

// read and display temperature method
void get_temperature()
{
  // get the value of the tmp36
  int tmp_val = analogRead(tmp_pin);
  
  // do temperature conversion
  float voltage = (tmp_val * 5.0) / 1023.0;
  float temperature = ((voltage - 0.5) * 1000.0) / 10.0;
  
  // debugging
  Serial.print("Temperature value: ");
  Serial.println(temperature, DEC);
}
