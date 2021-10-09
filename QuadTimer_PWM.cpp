/*
A test of QuadTimer Teensy 4 library, RollOver Interrupt is not working :(

A PWM test with QuadTimer on pin 4.

xbar is completely working for gpios and need to figure out for other internal modules
*/

#include <Arduino.h>

volatile int count = 0;

// taken from teensy 4 core
void xbar_connect(unsigned int input, unsigned int output)
{
  if (input >= 88) return;
  if (output >= 132) return;
  volatile uint16_t *xbar = &XBARA1_SEL0 + (output / 2);
  uint16_t val = *xbar;
  if (!(output & 1)) {
    val = (val & 0xFF00) | input;
  } else {
    val = (val & 0x00FF) | (input << 8);
  }
  *xbar = val;
}

void callback() {
  if (TMR3_SCTRL0 & (1 << 15)){
    TMR3_SCTRL0 &= ~(1 << 15); // clear the flag
    Serial.println("compare");
  }
  if(TMR3_SCTRL0 & (1 << 13)) {
    TMR3_SCTRL0 &= ~(1 << 13); // clear the flag
    Serial.println("rollover");
  }
}

void QuadTimer_Init() {
  // Enable the clock for quadtimer
  CCM_CCGR6 |= CCM_CCGR6_QTIMER1(CCM_CCGR_ON);
  TMR3_CTRL0 = 0; // stop
  TMR3_SCTRL0 = 0; // clear the register ** Important ** // external pin WON'T work IF NOT CLEARED 
  TMR3_SCTRL0 |=  TMR_SCTRL_OEN;
  TMR3_CSCTRL0 |= (1 << 0); // CL1
  TMR3_CTRL0 |= (0x06); // toggle on alternating compare
  TMR3_CTRL0 &= ~(1 << 4); // count up
  TMR3_CTRL0 &= ~(1 << 5); // count until 0xFFFF regardless of compare value
  TMR3_CTRL0 &= ~(1 << 6); // count repeatedly
  TMR3_CTRL0 |= (1 << 12); // primary count source which takes in IP clock with prescaler = 1
  TMR3_CTRL0 |= (1 << 14); // count both rising and falling edges of the clock
  TMR3_SCTRL0 &= ~(1 << 2);
  TMR3_SCTRL0 &= ~(1 << 3);
  TMR3_LOAD0 = 0;
  TMR3_COMP10 = 1500 - 1;
  TMR3_CMPLD10 = 1500 - 1;
  TMR1_SCTRL0 |= (1 << 14); // interrupt enable
  TMR3_SCTRL0 |= (1 << 2); // interrupt for rollover
  // TMR1_SCTRL0 |= (0x01) | (1 << 1);
  TMR3_ENBL |= (0x01); // Enable the timer channel
  
  // config pin
  // *(portConfigRegister(10)) = 1;  // ALT 1
}

void IOMUX_Init() {
  CCM_CCGR2 |= CCM_CCGR2_XBAR1(CCM_CCGR_ON);

  // configure pin 4 as output
  IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_06 &= ~(0x07); // xbar inout 8 mode
  IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_06 |= (0x03); // xbar inout 8 mode
  IOMUXC_GPR_GPR6 = (1 << 20); // xbar output
  IOMUXC_XBAR1_IN08_SELECT_INPUT = 0; // daisy chain register
  xbar_connect(32, 8);
}



void setup() {
  Serial.begin(9600);
  CCM_CCGR6 |= CCM_CCGR6_QTIMER1(CCM_CCGR_ON);
  QuadTimer_Init();
  IOMUX_Init();
  // pin 10
  IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_00 &= ~(0x07);
  IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_00 |= (0x01);

  // pin 19
  // IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_00 &= ~(0x07);
  // IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_00 |= (0x01);
  attachInterruptVector(135, callback);
  NVIC_ENABLE_IRQ(135);
}

void loop() {

}
