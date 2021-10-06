/*
An example of chained timers using PIT
*/

#include <Arduino.h>

// Custom delay 
void Delay_ms() {
  while (!(PIT_TFLG0 & (0x01))) {
    ;
  }
  PIT_TFLG0 |= (1 << 0); // clear interrupt flag
}

void callback() {
  if (PIT_TFLG0 & (1 << 0)) {
    PIT_TFLG0 |= (1 << 0); // clear interrupt flag
    Serial.println("Hi from counter 0");
  }
  else if (PIT_TFLG1 & 0x01) {
    PIT_TFLG1 |= (1 << 0); // clear interrupt flag
    Serial.println("Hi from counter 1");
  }
  
}

void PIT_Init() {
  // Enable the clock module for PIT
  CCM_CCGR1 |= CCM_CCGR1_PIT(CCM_CCGR_ON);

  // configure PIT
  PIT_MCR &= ~(0x02);
  PIT_LDVAL0 = 24000000;
  PIT_TCTRL0 |= (1 << 1) | (1 << 0);

  PIT_LDVAL1 = 5;
  PIT_TCTRL1 |= (0x07);

  attachInterruptVector(122, callback);
  NVIC_ENABLE_IRQ(122);
}

void setup() {
  Serial.begin(9600);
  PIT_Init();
}

void loop() {
}