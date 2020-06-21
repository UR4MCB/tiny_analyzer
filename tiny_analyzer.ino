/*Sketch for the work of Arduino UNO and the AA-30 ZERO analyzer without using a PC.
The analyzer operates in the frequency range from 1 to 30 MHz.
The operating frequency is set by rotating the encoder. Measurement is started by pressing the encoder knob once.
One click of the encoder turn changes the frequency by 10 kHz,
one click of the turn with push of encoder knob changes the frequency by 1 MHz.
Information is displayed on the LCD type 1602 I2C.
Used libraries:
  For display operation:https://rigexpert.com/files/libraries/LiquidCrystal_I2C_V112/
  For the analyzer to work: https://rigexpert.com/files/libraries/RigExpertZero/
  For encoder operation: https://rigexpert.com/files/libraries/GyverEncoder/
                         https://rigexpert.com/files/libraries/encTest/
© Alexander Antonov UR4MCB February 05, 2020.*/


/* Added a library to work with the display. */
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/* Display setting. Address I2C, number of columns, number of rows. */
LiquidCrystal_I2C lcd(0x27, 16, 2);

/* Added a library to work with AA-30 ZERO analyzer */
#include "RigExpertZero.h"

/* ZERO pins */
#define RX_PIN 4
#define TX_PIN 7

RigExpertZero ZERO(RX_PIN, TX_PIN);

/* Added a library to work with rotary encoder HW-040 */
#include "encMinim.h"

/* encoder pins */
#define SW 5
#define DT 3
#define CLK 2

/* setting the center frequency of the measurement, Hz */
volatile int32_t freq = 15000000;

/* variable "timer" so as not to use the standard delay function */
int32_t timer = millis();

/* pin CLK, pin DT, pin SW, direction of rotation (0/1), type (0/1) */
encMinim enc(CLK, DT, SW, 1, 0);

void setup() {

  ZERO.startZero();                   // Initialize analyzer
  delay(50);                          // required delay
  lcd.init();                         // Initialize display
  delay(10);                          // required delay
  lcd.backlight();                    // Turn on display backlight

/* Check if the analyzer is found? */
  while (!ZERO.startZero()) {
    lcd.setCursor(4, 0);              // set the cursor
    lcd.print("analyzer");            // and print text on the display
    lcd.setCursor(3, 1);
    lcd.print("not found");
    delay(1000);                      // recheck after 1 second
  }

/* If found, then display the greeting on the screen. */
  lcd.setCursor(3, 0);
  lcd.print("AA-30 ZERO");
  lcd.setCursor(4, 1);
  lcd.print("analyzer");
  delay(4000);                         // Display splash screen 4 seconds
  lcd.clear();                         // Clear display

  lcd.setCursor(0, 1);
  lcd.print("press to measure");


}


void loop() {
  enc.tick();                           // Initialize rotary encoder


  /* Rotary encoder processing */
  if (enc.isLeft()) {
    freq_left();
  }
  if (enc.isRight()) {
    freq_right();
  }
  if (enc.isLeftH()) {
    freq_leftH();
  }
  if (enc.isRightH()) {
    freq_rightH();
  }
  if (enc.isClick()) {
    start_m ();
  }

  /* Update the frequency value on screen every 200 ms. */
  if (millis() - timer > 200) {
    if ((freq / 1000) > 10000) {
      lcd.setCursor(0, 0);
      lcd.print("FQ = ");
      lcd.setCursor(5, 0);
      lcd.print(freq / 1000);
      lcd.setCursor(12, 0);
      lcd.print("kHz");
      timer = millis();
    }
    if ((freq / 1000) < 9999) {
      lcd.setCursor(0, 0);
      lcd.print("FQ = ");
      lcd.setCursor(9, 0);
      lcd.print(' ');
      lcd.setCursor(5, 0);
      lcd.print(freq / 1000);
      lcd.setCursor(12, 0);
      lcd.print("kHz");
      timer = millis();
    }
  }


}

void freq_leftH () {
  freq = freq + 1000000;
  if (freq >= 30000000) {
    freq = 30000000;
  }
}

void freq_rightH () {
  freq = freq - 1000000;
  if (freq <= 1000000) {
    freq = 1000000;
  }
}

void freq_left () {
  freq = freq + 10000;
  if (freq >= 30000000) {
    freq = 30000000;
  }
}

void freq_right () {
  freq = freq - 10000;
  if (freq <= 1000000) {
    freq = 1000000;
  }


}

void start_m () {
  ZERO.startMeasure(freq);              // start measurement
  delay(10);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("SWR");
  float SWR = ZERO.getSWR();            // get SWR value
  lcd.setCursor(4, 1);

  if (SWR < 10) {
    lcd.print(SWR, 2);
  }
  if ((SWR >= 10) && (SWR < 100)) {
    lcd.print(SWR, 1);
  }
  if ((SWR >= 100) && (SWR <= 200)) {
    lcd.print(">100");
  }

  int Z = ZERO.getZ();                  // get Z value
  lcd.setCursor(9, 1);
  lcd.print('Z');
  lcd.setCursor(11, 1);
  if (Z <= 1000) {
    lcd.print(Z);
  }
  if (Z > 1000 ) {
    lcd.print(">1000");
  }

}
