#ifndef TUNNELS_DCSG_H_
#define TUNNELS_DCSG_H_

#include "Timer.h"

namespace Tunnels { namespace DCSG {

constexpr unsigned CLOCK_HZ = 3579545;

class Synthesizer {
  friend class ScopedSynthesis;
protected:
  virtual void soundByte(byte n) = 0;
  virtual void endSynthesis() { };
};

class PCMSynthesizer : public Synthesizer {
private:
  unsigned sampleRate;
  unsigned lastReg;
  unsigned toneDivider[3];
  unsigned noiseDivider;
  unsigned noiseShift;
  int16 volume[4];
  uint32 period[4], delay[4];
  unsigned char enabled, voiceOn;
  int16 attenuatedVolume[16];
protected:
  PCMSynthesizer(unsigned sampleRate_);
  virtual void soundByte(byte n) override;
  void generatePCM(int16 *buffer, int len);
};

class ScopedSynthesis {
private:
  Synthesizer *target;
public:
  ScopedSynthesis(Synthesizer *target_) : target(target_) { }
  ~ScopedSynthesis() { target->endSynthesis(); }
  void soundByte(byte n) {
    target->soundByte(n);
  }
  void sound(int voice, int freq, int attenuation);
  ScopedSynthesis & operator=(const ScopedSynthesis&) = delete;
  ScopedSynthesis(const ScopedSynthesis&) = delete;
  ScopedSynthesis() = delete;
  ScopedSynthesis(ScopedSynthesis&& other) : target(other.target)
  { other.target = nullptr; }
};

class Backend {
 public:
  virtual ScopedSynthesis startSynthesis() = 0;
  virtual ~Backend() { }
};

class Music : Timer::Timer {
private:
  const byte *data;
  unsigned pos, length, timing;
  bool postEvent;
  Backend &backend;
  Tunnels::Timer::TimerManager &timerManager;

public:
  Music(Backend &backend_, Tunnels::Timer::TimerManager &timerManager_) :
    data(nullptr), pos(0), length(0), timing(0),
    backend(backend_), timerManager(timerManager_) {}
  template <unsigned n> void play(const byte (&a)[n], bool post=false,
				  unsigned offs=0)
  {
    play(&a[0], n, offs, post);
  }

protected:
  void callback() override;
private:
  void play(const byte *data, unsigned length, unsigned pos, bool post);
  void submitSoundBytes(const byte *data, unsigned length);
  int update();
};

}}

#endif // TUNNELS_DCSG_H_
