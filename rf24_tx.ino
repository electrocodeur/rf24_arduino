#include "Arduino.h"
#include <SPI.h>
#include <RF24.h>

/*
   NRF24L01     Arduino Uno  Arduino Mega  Blue_Pill(stm32f01C)
   ___________________________________________________
   VCC  RED    |    3.3v   |     3.3v       |      3.3v
   GND  BROWN  |    GND    |     GND        |      GND
   CSN  YELOW  |   Pin10   |     Pin10      | A4 NSS1   (PA4)  \
   CE   ORANGE |   Pin9    |     Pin9       | B0 digital(PB0)    |     NB
   SCK  GREEN  |   Pin13   |     Pin52      | A5 SCK1   (PA5)  |- All these pins
   MOSI BLUE   |   Pin11   |     Pin51      | A7 MOSI1  (PA7)  |  are 3.3v tolerant
   MISO PURPLE |   Pin12   |     Pin50      | A6 MISO1  (PA6)  /

   TOUJOURS ALIMENTER LE MODULE EN 3.3V

*/

RF24 radio(9, 10);

byte addresses[][6] = {"Addr1", "Addr2"};

#define axe_x  A0

struct data {
  int x;
};

data myData;
 
void setup() {
  Serial.begin(9600);
  Serial.println("THIS IS THE TRANSMITTER CODE - YOU NEED THE OTHER ARDIUNO TO SEND BACK A RESPONSE");
  pinMode(axe_x,  INPUT);
  // Initiate the radio object
  radio.begin();

  // Set the transmit power to lowest available to prevent power supply related issues
  radio.setPALevel(RF24_PA_MIN);

  //Vitesse de transmission etre 250KBPS et 2MBPS, plus la vitesse est grande moins est la portee
  radio.setDataRate(RF24_2MBPS);

  // Eviter de mettre le channel entre 0 et 100 parce que le wifi peut creer des interferences
  radio.setChannel(124);

  // Open a writing and reading pipe on each radio, with opposite addresses
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1, addresses[0]);

  // Random number seeding (we're going to be sending a single random number)
  randomSeed(analogRead(A0));
}

void loop() {
  int valx = analogRead(axe_x);
  myData.x = valx;

  // Ensure we have stopped listening (even if we're not) or we won't be able to transmit
  radio.stopListening();

  // Did we manage to SUCCESSFULLY transmit that (by getting an acknowledgement back from the other Arduino)?
  // Even we didn't we'll continue with the sketch, you never know, the radio fairies may help us
  if (!radio.write(&myData, sizeof(int))) {
    Serial.println("Pas de donnes - module radio reception connecte?");
  } else {
    Serial.println("TX: ACK");
  }
  // Now listen for a response
  radio.startListening();

  // But we won't listen for long, 200 milliseconds is enough
  unsigned long started_waiting_at = millis();

  // Loop here until we get indication that some data is ready for us to read (or we time out)
  while ( ! radio.available() ) {

    if (millis() - started_waiting_at > 200 ) {
      Serial.println("No response received - timeout!");
      return;
    }
  }

  // Now read the data that is waiting for us in the nRF24L01's buffer
  int dataRx;
  radio.read( &dataRx, sizeof(dataRx) );
  Serial.print("Joystick: ");
  Serial.print(myData.x);
  Serial.print(", received: ");
  Serial.println(dataRx);

  // Try again 1s later
  delay(1000);
}
