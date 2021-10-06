/*  
This code is a test to get GPT working in free run mode with
resetting the counter using ENMOD bit in GPT_CR register
*/ 

#include <Arduino.h>

// interrupt function
void GPT1_isr();

// getting delay in restart mode
void Delay_ms() {
  // check if the flag is set
  while (!(GPT1_SR & 0x01)) {
    ;
  }
  GPT1_SR |= (1 << 0); // clear the flag
}

// if interrupt enable
void GPT1_isr() {
  if (GPT1_SR & 0x01) {
    GPT1_SR |= (1 << 0); // clear flag
    Serial.println("hi1");
  }
  else if(GPT1_SR & 0x02) {
    GPT1_SR |= (1 << 1); // clear flag
    Serial.println("hi2");
  }
  else if(GPT1_SR & 0x04){
    // clear flag
    GPT1_SR |= (1 << 2);
    Serial.println("hi3");
    
    // resetting the counter
    GPT1_CR &= ~(1 << 0);
    GPT1_CR |= (1 << 1);
    GPT1_CR |= (1 << 0);
  }
}


// general purpose timers
void GPT_init() {
  // Enable clock source for GPT
  CCM_CCGR1 |= CCM_CCGR1_GPT(CCM_CCGR_ON);
  GPT1_CR = 0;
  GPT1_SR = 0x3F; // status
  GPT1_PR &= ~(0xFFF); // set prescaler to 1 // goes from 1-4096, choose your preference
  GPT1_OCR1 = (24000000 - 1);
  GPT1_OCR2 = (48000000 - 1);
  GPT1_OCR3 = (72000000 - 1);
  GPT1_IR |= (0x07); 


  // select clk and select free run mode
  GPT1_CR |= (1 << 6);
  GPT1_CR |= (1 << 9);

  // pending enable GPT
  GPT1_CR |= (1 << 0);

  // handle interrupts
  // CM7 interrupts table
  attachInterruptVector(100, GPT1_isr);
  NVIC_ENABLE_IRQ(100);
}

void setup() {
  Serial.begin(9600);
  GPT_init();
}

void loop() {
  
}
