/*
 * analog_blink_rate_with_calibration.ino
 *
 * simple program to adjust the blink rate of an led based
 * upon the value of a potentiometer
 * 
 * now with calibration
 */

// set pin numbers
const int led_pin = 11;
const int sensor_pin = A0;

// initialise the state of the led
int led_state = LOW;

// calibration variables
int sensor_value = 0;         // the current sensor value
int sensor_min = 1023;        // the minimum sensor value
int sensor_max = 0;           // the maximum sensor value

// CODE

// calibration routine (runs during setup() method)
void calibrate()
{
  // turn on led to signal the start of the calibration
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  // calibrate during the first ten seconds
  while (millis() < 10000)
  {
    sensor_value = analogRead(sensor_pin);

    // record the maximum sensor value
    if (sensor_value > sensor_max)
    {
      sensor_max = sensor_value;
    }

    // record the minimum sensor value
    if (sensor_value < sensor_min)
    {
      sensor_min = sensor_value;
    }
  }

  // signal the end of the calibration period
  digitalWrite(13, LOW);
}

// set up code
void setup()
{
  // set up the serial connection to enable debugging
  Serial.begin(9600);

  // set the pin i/o
  pinMode(led_pin, OUTPUT);

  // run calibration for the sensor
  Serial.println("calibrating ...");
  calibrate();
  Serial.println("done!");
}

// main program
void loop()
{
  // read the sensor
  sensor_value = analogRead(sensor_pin);

  // remap the sensor value using calibration information
  sensor_value = map(sensor_value, sensor_min, sensor_max, 0, 100);

  // if the sensor value is outside the calibrated range then constrain
  sensor_value = constrain(sensor_value, 0, 100);

  // debugging
  Serial.print("Potentiometer percentage: ");
  Serial.println(sensor_value, DEC);

  // flip the state of the LED
  if (led_state == HIGH)
  {
    led_state = LOW;
  }
  else
  {
    led_state = HIGH;
  }

  // then write it out and wait for the delay set by the pot
  digitalWrite(led_pin, led_state);
  delay(sensor_value * 100);

}
