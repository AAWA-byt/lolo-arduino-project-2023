// Import libaryrs
#include <SPI.h>
#include <SD.h>
#include <IRremote.h>

// Define pins for LEDs, ultrasonic sensor, and IR receiver
int GREEN_LED = 5;
int YELLOW_LED = 6;
int RED_LED = 3;
int PIN_TRIGGER = 8;
int PIN_ECHO = 7;
int RECV_PIN = 2;

// Set variable for tracking whether system is active
int ACTIVE = false;

// Variables for storing time and distance data
int time_1;
int time_2;
double time_3;
double distance_3;
double velocity;

// Initialize IR receiver object
IRrecv irrecv(RECV_PIN);
decode_results results;

// Set maximum range for ultrasonic sensor
const int SENSOR_MAX_RANGE = 300;

// Initialize variables for storing ultrasonic sensor data
unsigned long duration;
unsigned int distance;
int distance_1;
int distance_2;

// Initialize file object for writing data to SD card
File dataFile;

void setup() {

  // Set pin modes for LEDs, ultrasonic sensor, and IR receiver
  pinMode(PIN_TRIGGER, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  irrecv.enableIRIn();

  // Initialize serial communication
  Serial.begin(9600);

  // Wait for serial port to connect
  while (!Serial) {
    ;
  }

  // Initialize SD card
  Serial.print("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1)
      ;
  }
  Serial.println("initialization done.");

  // Turn off all LEDs to start
  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
}

void loop() {

  // Check for incoming IR signal
  if (irrecv.decode(&results)) {
    Serial.println(results.value, DEC);
    // Check if system is already active
    if (ACTIVE != true) {
      // Check if incoming signal is the correct code
      if (results.value == 4294967295) {
        // Set system to active
        ACTIVE = true;

        // Flash LEDs to indicate system is starting
        delay(1000);
        digitalWrite(RED_LED, HIGH);
        delay(1000);
        digitalWrite(YELLOW_LED, HIGH);
        delay(1000);

        // Turn on green LED to indicate system is ready
        digitalWrite(RED_LED, LOW);
        digitalWrite(YELLOW_LED, LOW);
        digitalWrite(GREEN_LED, HIGH);

        // Record time and distance data for first ultrasonic sensor reading
        time_1 = millis();
        dataFile = SD.open("test.txt", FILE_WRITE);
        digitalWrite(PIN_TRIGGER, LOW);
        delayMicroseconds(2);
        digitalWrite(PIN_TRIGGER, HIGH);
        delayMicroseconds(10);
        duration = pulseIn(PIN_ECHO, HIGH);
        distance = (duration / 2) * 0.03432;
        distance_1 = distance;

        // Print and write data to SD card
        Serial.println("___________");
        Serial.println("Mark 1:");
        Serial.println(String(distance_1) + " cm");
        Serial.println(String(time_1));

        if (dataFile) {
          dataFile.println("___________");
          dataFile.println("Mark 1:");
          dataFile.println("Distance: " + String(distance_1));
          dataFile.println("Time: " + String(time_1));

          dataFile.close();
        } else {
          Serial.println("Could not write to file.");
        }

        while (distance > 5) {
          digitalWrite(PIN_TRIGGER, LOW);  // set trigger pin to low
          delayMicroseconds(2);            // wait for 2 microseconds

          digitalWrite(PIN_TRIGGER, HIGH);  // set trigger pin to high
          delayMicroseconds(10);            // wait for 10 microseconds

          duration = pulseIn(PIN_ECHO, HIGH);   // read duration of the pulse
          distance = (duration / 2) * 0.03432;  // calculate distance in cm
        }

        time_2 = millis();  // get current time in milliseconds

        dataFile = SD.open("test.txt", FILE_WRITE);  // open file in write mode

        digitalWrite(PIN_TRIGGER, LOW);  // set trigger pin to low
        delayMicroseconds(2);            // wait for 2 microseconds

        digitalWrite(PIN_TRIGGER, HIGH);  // set trigger pin to high
        delayMicroseconds(10);            // wait for 10 microseconds

        duration = pulseIn(PIN_ECHO, HIGH);    // read duration of the pulse
        distance = (duration / 2) * 0.03432;   // calculate distance in cm
        distance_2 = distance;                 // set distance_2 to the current distance
        time_3 = time_2 - time_1;              // calculate time difference between mark 1 and mark 2
        distance_3 = distance_1 - distance_2;  // calculate distance difference between mark 1 and mark 2
        velocity = distance_3 / time_3;        // calculate velocity

        Serial.println("___________");
        Serial.println("Mark 2:");
        Serial.println(String(distance_2) + " cm");
        Serial.println(String(time_2));
        Serial.println("Velocity: " + String(velocity) + " cm/ms");

        if (dataFile) {                                                  // if file is open
          dataFile.println("___________");                               // write separator to file
          dataFile.println("Mark 2:");                                   // write mark 2 label to file
          dataFile.println("Distance: " + String(distance_2));           // write distance to file
          dataFile.println("Time: " + String(time_2));                   // write time to file
          dataFile.println("Velocity: " + String(velocity) + " cm/ms");  // write velocity to file

          dataFile.close();  // close file
        } else {
          Serial.println("Could not write to file.");  // print error message to serial monitor
        }

        ACTIVE = false;                 // set ACTIVE flag to false
        digitalWrite(RED_LED, LOW);     // turn off red LED
        digitalWrite(YELLOW_LED, LOW);  // turn off yellow LED
        digitalWrite(GREEN_LED, LOW);   // turn off green LED
      }
      irrecv.resume();  // resume IR receiver
    }
  }
}
  // The code below is commented out and not currently used
  // dataFile = SD.open("test.txt", FILE_WRITE);
  // digitalWrite(PIN_TRIGGER, LOW);
  // delayMicroseconds(2);
  // digitalWrite(PIN_TRIGGER, HIGH);
  // delayMicroseconds(10);
  // duration = pulseIn(PIN_ECHO, HIGH);
  // distance = (duration/2) * 0.03432;
  // Serial.println("Distance to object: " + String(distance) + " cm");
  // if (dataFile) {
  //     dataFile.println(distance);
  //     dataFile.close();
  // } else {
  //     Serial.println("Could not write to file.");
  // }