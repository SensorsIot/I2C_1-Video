/* Showcase of how to connect several I2C devices to one bus

- Pressure Sensor
- OLED display
- Capacitive Sensor

*/

// Your sketch must #include this library, and the Wire library.
// (Wire is a standard library included with Arduino.):

#include <SPI.h>
#include <SFE_BMP180.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_MPR121.h"

// You will need to create an SFE_BMP180 object, here called "pressure":

#define OLED_RESET 4
Adafruit_SSD1306 display1(OLED_RESET);
SFE_BMP180 pressure;
Adafruit_MPR121 cap = Adafruit_MPR121();

double T, P;
bool page = false;


void setup()
{
  Serial.begin(115200);
  Serial.println("REBOOT");
  display1.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // Initialize the sensor (it is important to get calibration values stored on the device).

  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.

    Serial.println("BMP180 init fail (disconnected?)\n\n");
    while (1); // Pause forever.
  }

  Serial.println("Adafruit MPR121 Capacitive Touch sensor test");

  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");

}

void loop()
{
  // Get a new pressure reading:
  getPressure();

  if (  cap.touched() == 1 ) page = !page;

  if (page) {

    display1.setTextSize(2);
    display1.setTextColor(WHITE);
    display1.clearDisplay();
    display1.setCursor(10, 0);
    display1.println("Pressure");
    display1.setCursor(10, 30);
    display1.println(P);
    display1.display();

  }
  else {

    display1.setTextSize(2);
    display1.setTextColor(WHITE);
    display1.clearDisplay();
    display1.setCursor(10, 0);
    display1.println("Temp");
    display1.setCursor(10, 30);
    display1.println(T);
    display1.display();
  }

  delay(500);
}


void getPressure()
{
  char status;
  double p0, a;

  // You must first get a temperature measurement to perform a pressure reading.

  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.

  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:

    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Use '&T' to provide the address of T to the function.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.

      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Use '&P' to provide the address of P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = pressure.getPressure(P, T);
        if (status != 0)
        {

        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");
}
