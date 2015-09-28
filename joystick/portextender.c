/*
 * portextender.c -- I/O multiplexer for ledmeplay
 *
 * Copyright (C) 2015, juewei@fabfolk.com, distribute under GPL, use with mercy.
 *
 */
#include "cpu_mhz.h"

#include <util/delay.h>			// needs F_CPU from cpu_mhz.h
#include <avr/io.h>
#include "pitchlist.h"

#define SPEAKER_PORT PORTB
#define SPEAKER_DDR  DDRB
#define SPEAKER_BIT  (1<<PB4)

#if (CPU_MHZ != 8)
# error "must run at 8Mhz"
#endif

static uint16_t pitchval(uint8_t i)
{
  uint16_t val = (uint16_t)pgm_read_word(pitchlist + i);
  return val;
}

static void set_freq(uint8_t n)
{
  // n=0xff disables the output.
  // n=0..PITCHLIST_LEN beep with frequency from pitchlist[n]
  if (n >= PITCHLIST_LEN)
    {
      DDRB &= ~(1<<PB4);
    }
  else
    {
      DDRB |= (1<<PB4);
      OCR1A = pitchval(n);
      TCNT1 = 0;
    }
}

static void init_timer1()
{
  // CTC mode: WGM 13:0 = 4
  // output toggle mode COM1B1:0 = 1
  OCR1B = 1;	// 0 possible?
  TCCR1A = (0<<COM1A1)|(0<<COM1A0)|(0<<COM1B1)|(1<<COM1B0) | (0<<WGM11)|(0<<WGM10);
  // Prescaler=8 CS=0,1,0; prescaler=1 CS=0,0,1
  TCCR1B = (0<<ICNC1)|(0<<ICES1) | (0<<WGM13)|(1<<WGM12) | (0<<CS12)|(1<<CS11)|(0<<CS10);
  set_freq(0xff);
}

int main()
{
  uint8_t i;
  uint8_t j;

  init_timer1();

  for (j = 0; j < 4; j++)
    {
      for (i = 0; i < PITCHLIST_LEN; i++)
	{
	  set_freq(i);    _delay_ms(20.0); 
          set_freq(0xff); _delay_ms(5.0);
	}
      for (i = 0; i < PITCHLIST_LEN; i++)
	{
	  set_freq(PITCHLIST_LEN-1-i);    _delay_ms(20.0); 
          set_freq(0xff); _delay_ms(5.0);
	}
    }
  set_freq(0xff);

  return 0;
}

