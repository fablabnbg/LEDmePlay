#!/usr/bin/python
#
# (c) 2015 juewei@fabfolk.com

import math

#clock=16000000		# 16Mhz
clock=8000000		# 8Mhz
prescaler=8		# 1,8,64,256,1024
lowest=440./(2**5)	# Hz Start 5 octaves under 440hz
# lowest=440./(2**3)	# Hz Start 3 octaves under 440hz

# 12 chromatic intervals per octave
# https://en.wikipedia.org/wiki/Twelfth_root_of_two
def halftone(n=1, base=lowest):
  return base * pow(2, float(n)/12 )

def halfhalftone(n=1, base=lowest):
  return base * pow(2, float(n)/24 )

def freq_atmega(top=65535, N=prescaler, ioclock=clock):
  return float(ioclock)/(2*N*top)

def freq2top_atmega(freq=lowest, N=prescaler, ioclock=clock):
  return float(ioclock)/(2*N*freq)

a=[]
idx440hz=None
for i in range(127):
  f=halftone(i)
  t=int(freq2top_atmega(f)+.5)
  if (t < 65536 and t > 1 and f < 20000.):
    if idx440hz is None and f >= 440.: idx440hz = len(a)
    a.append(t)
    # print i, f, t

print "#include <avr/pgmspace.h>"
print "#define PITCHLIST_MIN_HZ %.1f" % freq_atmega(a[0])
print "#define PITCHLIST_MAX_HZ %.1f" % freq_atmega(a[-1])
if idx440hz:
  print "#define PITCHLIST_440_HZ_IDX "+str(idx440hz)
print "#define PITCHLIST_LEN "+str(len(a))
print "const uint16_t pitchlist[PITCHLIST_LEN] PROGMEM = { "+", ".join(map(lambda x: str(x), a))+" };"

