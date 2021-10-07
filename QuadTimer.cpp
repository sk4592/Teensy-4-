/*
A test of QuadTimer Teensy 4, RollOver Interrupt is not working :(
*/

#include <Arduino.h>

volatile int count = 0;

void callback() {
  if (TMR1_SCTRL0 & (1 << 15)){
    TMR1_SCTRL0 &= ~(1 << 15); // clear the flag
    Serial.println("compare");
  }
  if(TMR1_SCTRL0 & (1 << 13)) {
    TMR1_SCTRL0 &= ~(1 << 13); // clear the flag
    Serial.println("rollover");
  }
}

void QuadTimer_Init() {
  // Enable the clock for quadtimer
  CCM_CCGR6 |= CCM_CCGR6_QTIMER1(CCM_CCGR_ON);

  TMR1_CTRL0 = 0;
  TMR1_CTRL0 |= (0x06); // toggle on alternating compare
  TMR1_CTRL0 &= ~(1 << 4); // count up
  TMR1_CTRL0 &= ~(1 << 5); // count until 0xFFFF regardless of compare value
  TMR1_CTRL0 &= ~(1 << 6); // count repeatedly
  TMR1_CTRL0 |= (1 << 12); // primary count source which takes in IP clock with prescaler = 1
  TMR1_CTRL0 |= (1 << 13); // count both rising and falling edges of the clock
  TMR1_LOAD0 = 0; // load this value on compare
  TMR1_COMP10 = 1500;
  TMR1_CMPLD10 = 1500; // this value gets loaded into compare register 1
  TMR1_SCTRL0 |= (1 << 14); // interrupt enable
  TMR1_SCTRL0 |= (1 << 12); // interrupt for rollover
  TMR1_ENBL |= (0x01); // Enable the timer channel
}


void setup() {
  Serial.begin(9600);
  QuadTimer_Init();
  attachInterruptVector(133, callback);
  NVIC_ENABLE_IRQ(133);
}

void loop() {
}
