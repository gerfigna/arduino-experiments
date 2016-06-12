
/*
  Getting Started example sketch for nRF24L01+ radios
  This is a very basic example of how to send data from one node to another
  Updated: Dec 2014 by TMRh20
*/

#include <SPI.h>
#include "RF24.h"

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7, 8);
byte addresses[][6] = {"1Node", "2Node"};

const int buttonPin = 2;     // the number of the pushbutton pin
int buttonState = 0;         // variable for reading the pushbutton status
int lastButtonState = 0;
int sensorPin = A0;
int sensorValue = 0;
int treshold = 400;

void setup() {
  Serial.begin(115200);

  radio.begin();


  // Set the PA Level low to prevent power supply related issues since this is a
  // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_MAX);


  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);

  pinMode(buttonPin, INPUT);

}

void loop() {

  buttonState = digitalRead(buttonPin);
  sensorValue = analogRead(sensorPin);

  if ( buttonState == HIGH | sensorValue > treshold) {
    if (lastButtonState == LOW) {

      /****************** Ping Out Role ***************************/
      radio.stopListening();                                    // First, stop listening so we can talk.

      unsigned long start_time = micros();                             // Take the time, and send it.  This will block until complete
      while (!radio.write( &start_time, sizeof(unsigned long) )) {
        Serial.println(F("failed"));
      }

      radio.startListening();                                    // Now, continue listening

      unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
      boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not

      while ( ! radio.available() ) {                            // While nothing is received
        if (micros() - started_waiting_at > 200000 ) {           // If waited longer than 200ms, indicate timeout and exit while loop
          timeout = true;
          break;
        }
      }

      if ( timeout ) {                                            // Describe the results
        Serial.println(F("Failed, response timed out."));
      } else {
        unsigned long got_time;                                 // Grab the response, compare, and send to debugging spew
        radio.read( &got_time, sizeof(unsigned long) );
        unsigned long end_time = micros();

        // Spew it
        Serial.print(F("Sent "));
        Serial.print(start_time);
        Serial.print(F(", Got response "));
        Serial.print(got_time);
        Serial.print(F(", Round-trip delay "));
        Serial.print(end_time - start_time);
        Serial.println(F(" microseconds"));
      }
    }
    lastButtonState = HIGH;
  }
  else {
    lastButtonState = LOW;
  }

} // Loop

