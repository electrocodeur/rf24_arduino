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


struct data {
  int x;
};

data myDataRx;

void setup() {
  Serial.begin(9600);
  Serial.println("THIS IS THE RECEIVER CODE - YOU NEED THE OTHER ARDUINO TO TRANSMIT");

  // Initiate the radio object
  radio.begin();

  // Set the transmit power to lowest available to prevent power supply related issues
  radio.setPALevel(RF24_PA_MIN);

  //Vitesse de transmission etre 250KBPS et 2MBPS, plus la vitesse est grande moins est la portee
  radio.setDataRate(RF24_2MBPS);

  // Eviter de mettre le channel entre 0 et 100 parce que le wifi peut creer des interferences
  radio.setChannel(124);

  // Open a writing and reading pipe on each radio, with opposite addresses
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);

  // Start the radio listening for data
  radio.startListening();
}

void loop() {
  if ( radio.available()) {

    // Go and read the data and put it into that variable
    while (radio.available()) {
      //radio.read( &data, sizeof(char));
      radio.read(&myDataRx, sizeof(myDataRx));
    }

    // No more data to get so send it back but add 1 first just for kicks
    // First, stop listening so we can talk
    radio.stopListening();
    radio.write( &myDataRx, sizeof(myDataRx) );

    // Now, resume listening so we catch the next packets.
    radio.startListening();

    // Tell the user what we sent back (the random numer + 1)
    Serial.print("Re-envoie : ");
    Serial.println(myDataRx.x);

  }
}
