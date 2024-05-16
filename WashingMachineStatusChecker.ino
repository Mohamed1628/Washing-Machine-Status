/* 

  The following variables are automatically generated and updated when changes are made to the Thing

  int cycleTime;            Time that cycle takes (Ex. 30 minutes for a delicate cycle)
  bool cycleStatus;         Status of wash cycle (Complete [T] or Incomplete [F] )
  bool doorStatus;          Status of washer door (Open [T] or Closed [F] )
  CloudTime completeTime;   Time that the cycle finished
  CloudTime openTime;       Time that the washer door was opened

*/

#include "thingProperties.h"
#include "DHT.h"
#define DHTPIN 5
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

const int micPin = 17;                 // analog pin used for microphone data pin
int sound;                             // integer to store value from microphone
int humidity;                          // integer to store value from humidity sensor
const int audioThreshold = 700;        // threshold value that cycle complete tone should surpass
const int humidityThreshold = 96;      // threshold value that steam from open washer should surpass

void setup() {
  // Initialize serial and dht sensor:
  Serial.begin(9600); //9600 is baud rate
  dht.begin();
  delay(1500); 

  // Defined in thingProperties.h
  initProperties();
  pinMode(micPin, INPUT); //setting microphone pin as input
 
  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  
  cycleStatus = false;                 // initializing cycle status to false (cycle just started)
  doorStatus = false;                  // initializing door status to false (door should be closed initially)

  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update();
  float h = dht.readHumidity();
  sound = analogRead(micPin);          // reading in microphone values to variable sound
  humidity = h;                        // reading in humidity values to variable humidity

  // Uncomment to see sensor values for adjusting threshold
  // Serial.println("Sound: ");         
  // Serial.print(sound);
  // Serial.println("Humidity: ");
  // Serial.println(humidity);  
  
  // if sound is still below threshold then do nothing
  if (sound < audioThreshold) {
    // do nothing...
  } 
  
  // else set status to true
  else if (cycleStatus == false) {
    cycleStatus = true; //complete tone was played
    completeTime = ArduinoCloud.getLocalTime();
  }
    
  else {
    // do nothing since cycleStatus is already true or not above threshold
  }
  
  // if humidity is still below threshold then do nothing
  if (humidity < humidityThreshold) {
    //do nothing...
  } 
    
  // else set status to true
  else if (doorStatus == false) {
    doorStatus = true; //door is open
    openTime = ArduinoCloud.getLocalTime();
  } 
  else {
    // do nothing since doorStatus is already true or not above threshold
  }
  
}

void onCycleTimeChange()  {

  completeTime = ArduinoCloud.getLocalTime() + (cycleTime*60) - (0.001*millis()); 
  // current time + cycle time - offset of how long arduino has been running

  // For example:
  // Let's say true start time was 12:00pm
  // 12:05pm + 45 minute cycle = 12:50 - 5 minutes arduino has been running = 12:45pm
}