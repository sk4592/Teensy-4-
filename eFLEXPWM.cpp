/*
A test of eFlexPWM on pin 4 of Teensy 4.0
*/

#include <Arduino.h>

volatile int count = 0;

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

void eFlexPwm_Init() {
  // enable the clock to this module
  CCM_CCGR4 |= CCM_CCGR4_PWM1(CCM_CCGR_ON);
  FLEXPWM1_SM3CTRL2 |= (1 << 13); // independent outputs
  FLEXPWM1_SM3CTRL2 &= ~(0x03); // select IPBus clock
  FLEXPWM1_SM3CTRL &= ~(0x070); // clear
  FLEXPWM1_SM3CTRL |= (1 << 10) | (1 << 4); // set prescaler to f/2
  FLEXPWM1_SM3INIT = 0;
  FLEXPWM1_SM3VAL2 = 15000; // rise 
  FLEXPWM1_SM3VAL3 = 15000; // fall
  FLEXPWM1_SM3VAL1 = 60000; // final
  FLEXPWM1_SM3TCTRL |= (1 << 15);
  FLEXPWM1_SM3TCTRL |= (1 << 0) | (1 << 2) | (1 << 4);
  FLEXPWM1_OUTEN |= (1 << 11); // enable the output from pwm_A
  FLEXPWM1_MCTRL |= (1 << 3); // set LDOK
  FLEXPWM1_MCTRL |= (1 << 11); // set RUN 
}

void counter() {
  count++;
}

void setup() {
  CCM_CCGR2 |= CCM_CCGR2_XBAR1(CCM_CCGR_ON);
  eFlexPwm_Init();

  // pin 4 xbar output
  IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_06 &= ~(0x07); // xbar inout 8 mode
  IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_06 |= (0x03); // xbar inout 8 mode
  IOMUXC_GPR_GPR6 = (1 << 20); // xbar output
  IOMUXC_XBAR1_IN08_SELECT_INPUT = 0; // daisy chain register

  xbar_connect(43, 8);
}

void update(uint32_t value) {
  FLEXPWM1_MCTRL |= (1 << 7); // clear LDOK
  FLEXPWM1_SM3VAL3 = value;
  FLEXPWM1_MCTRL |= (1 << 3); // set LDOK
  FLEXPWM1_MCTRL |= (1 << 11);
}

void loop() {
  // fading
  for (uint32_t i = 15000; i <= 40000; i++) {
    update(i);
    i += 99;
    delay(10);
  }
  for (uint32_t i = 40000; i >= 15000; i--) {
    update(i);
    i -= 99;
    delay(50);
  }
}
