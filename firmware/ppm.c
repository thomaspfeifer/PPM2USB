//=========================================================================
//
// Project: PPM2USB, a AVR-based PPM to USB-Joystick Converter using V-USB.
// Author: Thomas Pfeifer
// Creation Date: 2010-09-10
//
// WWW: http://thomaspfeifer.net/ppm2usb_adapter.htm
//
// Copyright: (c) 2010, Thomas Pfeifer - www.thomaspfeifer.net
//
//            This software is free for non-commercial use. It may be copied,
//            modified, and redistributed provided that this copyright notice
//            is preserved on all copies.
//
//            You may NOT use this software, in whole or in part, in support
//            of any commercial product without the express consent of the
//            author.
//
//            There is no warranty or other guarantee of fitness of this
//            software for any purpose. It is provided solely "as is".
//
//=========================================================================

#include <avr/io.h>
#include <avr/interrupt.h>
#include "ppm.h"

#define PPM_TCNT TCNT1

#define CHANNELS (8)
volatile unsigned int ppm[CHANNELS];
volatile char chan=-1;

volatile char ppmNewData;

SIGNAL (SIG_OVERFLOW1) {
  if (chan!=-1) {
    ppmNewData=1;
  }
  chan=-1;
}

SIGNAL (SIG_INPUT_CAPTURE1) {
  unsigned int timer=PPM_TCNT;
  PPM_TCNT=0;
  if (chan>=0 && chan<CHANNELS) {
    ppm[chan]=timer;
  }
  chan++;
}


void ppmInit() {
#ifdef TIMSK
  // tested with ATMega8
  TCCR1B=(1<<ICES1)|(1<<CS10); // TClk=CPUClk (no prescaler), ICP on rising edge
  TIMSK=(1<<TICIE1)|(1<<TOIE1); //Overflow and ICP-Interrupt enable
#else
  // for ATMega48/ATMega88/ATMega168
  TCCR1B=(1<<ICES1)|(1<<CS10); // TClk=CPUClk (no prescaler), ICP on rising edge
  TIMSK1=(1<<ICIE1)|(1<<TOIE1); //Overflow and ICP-Interrupt enable
#endif
  ppmNewData=0;
}

unsigned char ppmGet(int n) {
  unsigned int t=ppm[n];
  if (t<12000) t=12000;
  t-=12000;
  //t=(u16)((255.0/12000.0)*(float)t); //slow!
  t=t/45;
  if (t>255) t=255;
  return ((unsigned char)t);
}

