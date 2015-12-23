#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

/* This driver uses the Adafruit unified sensor library (Adafruit_Sensor),
   which provides a common 'type' for sensor data and some helper functions.
   
   To use this driver you will also need to download the Adafruit_Sensor
   library and include it in your libraries folder.

   You should also assign a unique ID to this sensor for use with
   the Adafruit Sensor API so that you can identify this particular
   sensor in any data logs, etc.  To assign a unique ID, simply
   provide an appropriate value in the constructor below (12345
   is used by default in this example).
   
   Connections
   ===========
   Connect SCL to analog 5
   Connect SDA to analog 4
   Connect VDD to 3-5V DC
   Connect GROUND to common ground
    
   History
   =======
   2015/MAR/03  - First release (KTOWN)
*/

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (100)
   

Adafruit_BNO055 bno; /* = Adafruit_BNO055(55); */
/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(void)
{
  sensor_t sensor;
  bno.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" xxx");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" xxx");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" xxx");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
void setup(void) 
{
  Serial.begin(9600);
  Serial.println("Orientation Sensor Test"); Serial.println("");
  bno = Adafruit_BNO055(55);
  /* Initialise the sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  delay(1000);
    
  /* Display some basic information on this sensor */
  displaySensorDetails();

  bno.setExtCrystalUse(true);
}

void print_vec(sensors_vec_t vec) {
  Serial.print("X: ");
  Serial.print(vec.x, 4);
  Serial.print("\tY: ");
  Serial.print(vec.y, 4);
  Serial.print("\tZ: ");
  Serial.print(vec.z, 4);
  Serial.print("\t");
}

void print_vec(imu::Vector<3>* vec) {
  Serial.print("X: ");
  Serial.print(vec->x(), 4);
  Serial.print("\tY: ");
  Serial.print(vec->y(), 4);
  Serial.print("\tZ: ");
  Serial.print(vec->z(), 4);
  Serial.print("\tMag: ");
  Serial.print(vec->magnitude(), 4);
  Serial.print("\t");
  
}

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/
void loop(void) 
{
  /* Get a new sensor event */ 
  sensors_event_t event; 
  bno.getEvent(&event);
  
  /* Display the floating point data */
  
  Serial.print("Orientation ");
  print_vec(event.orientation);
  Serial.print("Acceleration ");
  print_vec(event.acceleration);

  // Linear Acceleration.
  imu::Vector<3> lineacc = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  if (lineacc.magnitude() > 5) {
    digitalWrite(13, HIGH);
  } else {
    digitalWrite(13, LOW);
  }
  Serial.print("Lin Acceleration ");
  print_vec(&lineacc);


  imu::Vector<3> grav = bno.getVector(Adafruit_BNO055::VECTOR_GRAVITY);
  
  Serial.print("Grav ");
  print_vec(&grav);
  Serial.println("");
  
  
  delay(BNO055_SAMPLERATE_DELAY_MS);
}
