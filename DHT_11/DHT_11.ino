#include "DHT.h"
#define DHT11 D2

DHT dht;
void setup()
{
  Serial.begin(9600);
  dht.setup(DHT11);
}
 
void loop()
{
  float const humidity = dht.getHumidity();
  Serial.print(humidity);
  Serial.print(" %RH | ");
  
  float const temperature = dht.getTemperature();
  Serial.print(temperature);
  Serial.println("*C");
  delay(1000);
}