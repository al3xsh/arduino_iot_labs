/*
 * thermostat.ino
 *
 * simple program to read the temperature from a tmp36 every 5s and
 * display on the serial monitor
 */

// set pin numbers
const int led_pin = 11;
const int tmp_pin = A0;
const int pot_pin = A1;

// state machine variables ...
long previous_time  = 0;
long query_interval = 5000;

// thermostat temperature
float setpoint = 0.0;

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
  // get the current time this time round the loop
  unsigned long current_time = millis();
  
  // if it's time to query the temperature sensor ...
  if ((current_time - previous_time) >= query_interval)
  {
    // save the time
    previous_time = current_time;
    
    // get the temperature and print it
    float temperature = get_temperature();    
    Serial.print("Temperature value: ");
    Serial.println(temperature, DEC);
    
    // get the value of the thermostat and print it
    float setpoint = get_setpoint();
    Serial.print("Thermostat set to: ");
    Serial.println(setpoint, DEC);
    
    // check if the
    if (temperature < setpoint)
    {
      digitalWrite(led_pin, HIGH);
    }
    else
    {
      digitalWrite(led_pin, LOW);      
    }
  }
}

// read temperature method
float get_temperature()
{
  // get the value of the tmp36 (we read the analog pin twice in quick succession
  // so that the reading remains stable - see arduino forums for more information)
  analogRead(tmp_pin);
  delay(10);
  int tmp_val = analogRead(tmp_pin);
  
  // do temperature conversion
  float voltage = (tmp_val * 5.0) / 1023.0;
  float temperature = ((voltage - 0.5) * 1000.0) / 10.0;
  
  return temperature;
}

// read temperature method
float get_setpoint()
{
  // get the value of the potentiometer (we read the analog pin twice in quick succession
  // so that the reading remains stable - see arduino forums for more information)
  analogRead(pot_pin);
  delay(10);
  int thermostat_val = analogRead(pot_pin);
  
  // remap to sensible limits (0 - 30)
  thermostat_val = map(thermostat_val, 0, 1023, 0, 30);
  
  return thermostat_val;
}
