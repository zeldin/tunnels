#include "system.h"

#include "DCSG.h"
#include "Timer.h"

#include <cmath>

namespace Tunnels { namespace DCSG {

void ScopedSynthesis::sound(int voice, int freq, int attenuation)
{
  if (voice == 3) {
    if (freq < -8 || freq > -1)
      return;
    target->soundByte(0xe0|~freq);
  } else if (voice > 3 || voice < 0)
    return;
  else {
    if (freq < 110)
      freq = 110;
    else if (freq > 44100)
      freq = 44100;
    freq = (CLOCK_HZ/unsigned(freq)) >> 5;
    target->soundByte(0x80|(voice<<5)|(freq&0xf));
    target->soundByte(freq>>4);
  }
  if (attenuation < 0)
    attenuation = 0;
  else if (attenuation > 30)
    attenuation = 30;
  target->soundByte(0x90|(voice<<5)|(attenuation>>1));
}

PCMSynthesizer::PCMSynthesizer(unsigned sampleRate_)
  : sampleRate(sampleRate_), lastReg(0), noiseDivider(0),
    enabled(0), voiceOn(0) {
  unsigned i;
  for (i=0; i<4; i++) {
    volume[i] = 0;
    period[i] = 0;
    delay[i] = 0;
    if (i != 3)
      toneDivider[i] = 0;
  }
  for (i=0; i<15; i++)
    attenuatedVolume[i] = 0x1fff * pow(10.0, -0.1*i);
  attenuatedVolume[15] = 0;
}

void PCMSynthesizer::soundByte(byte n)
{
  unsigned regno, regval;
  regval = n & 0xf;
  if ((n&0x80)) {
    lastReg = regno = (n>>4)&7;
    if (regno < 6 && !(regno & 1))
      regval |= (toneDivider[regno >> 1] & ~0xf);
  } else {
    regno = lastReg;
    if (regno < 6 && !(regno & 1))
      regval = ((n&0x3f) << 4) | (toneDivider[regno >> 1] & 0xf);
  }
  unsigned voice = regno >> 1;
  if (regno & 1) {
    // Attenuation
    volume[voice] = attenuatedVolume[regval];
  } else if (voice < 3) {
    // Tone frequency divider
    toneDivider[voice] = regval;
    period[voice] = (regval*sampleRate)<<4;
    if (voice == 2 && (noiseDivider & 3) == 3)
      period[3] = period[2]<<1;
  } else {
    // Noise frequency divider
    noiseDivider = regval;
    noiseShift = 1U << 14;
    switch (regval & 3) {
      // Period is doubled because register doesn't shift on both flanks
    case 0: period[3] = sampleRate<<9; break;
    case 1: period[3] = sampleRate<<10; break;
    case 2: period[3] = sampleRate<<11; break;
    case 3: period[3] = period[2]<<1; break;
    }
  }
  if (!volume[voice])
    enabled &= ~(1U << voice);
  else if (voice == 3) {
    if ((noiseDivider & 3) != 3 || toneDivider[2] != 0)
      enabled |= 1U << voice;
    else
      enabled &= ~(1U << voice);
  } else if (toneDivider[voice] != 0)
    enabled |= 1U << voice;
  else
    enabled &= ~(1U << voice);
}

void PCMSynthesizer::generatePCM(int16 *buffer, int len)
{
  unsigned i;
  unsigned on = voiceOn;
  unsigned noise = noiseShift;
  while (len>0) {
    int16 n = 0;
    for (i=0; i<4; i++)
      if (enabled & (1U << i)) {
	while (delay[i] < CLOCK_HZ) {
	  if (i == 3) {
	    unsigned feedback = ((noiseDivider & 4)?
				 (~(noise ^ (noise >> 1))) & 1 : /* XNOR */
				 (noise & 1)); /* Straight feedback */
	    noise = (noise >> 1) | (feedback << 14);
	    on = (on & 7) | ((noise & 1) << 3);
	  } else
	    on ^= (1U << i);
	  delay[i] += period[i];
	}
	delay[i] -= CLOCK_HZ;
	if (on & (1U << i))
	  n += volume[i];
      }
    *buffer++ = n;
    --len;
  }
  voiceOn = on;
  noiseShift = noise;
}

void Music::play(const byte *data_, unsigned length_, unsigned pos_)
{
  data = data_;
  length = length_;
  pos = pos_;
  timerManager.addTimer(*this);
}

int Music::update()
{
  if (!data || pos >= length)
    return 0;
  byte h = data[pos++];
  if (h == 0 || h == 0xff) {
    if (pos+2 > length)
      return 0;
    pos = (data[pos]<<8)|data[pos+1];
    return 1;
  }
  if (pos+h >= length)
    return 0;
  submitSoundBytes(data+pos, h);
  pos += h;
  return data[pos++];
}

void Music::submitSoundBytes(const byte *data, unsigned length)
{
  ScopedSynthesis s{backend.startSynthesis()};
  while (length--)
    s.soundByte(*data++);
}

void Music::callback()
{
  int n = update();
  if (!n) {
    data = nullptr;
    length = 0;
    pos = 0;
  } else {
    /* 60 Hz VDP interrupts translate into 16, 17, 17 ms intervals */
    uint32 next = getExpiry() + (n/3)*50;
    if ((n %= 3)) {
      next += n*17;
      if (timing == 0 || timing+n > 3)
	/* One 16 ms interval */
	--next;
      if ((timing += n) >= 3)
	timing -= 3;
    }
    timerManager.addTimer(*this, next);
  }
}

}}
