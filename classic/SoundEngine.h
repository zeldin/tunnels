#ifndef TUNNELS_CLASSIC_SOUNDENGINE_H_
#define TUNNELS_CLASSIC_SOUNDENGINE_H_

#include "Engine.h"
#include "DCSG.h"

namespace Tunnels {

namespace Classic {

class SoundEngine : public Tunnels::SoundEngine {
private:
  Timer::TimerManager &timerManager;
  DCSG::Music music;

public:
  SoundEngine(DCSG::Backend &backend_, Timer::TimerManager &timerManager_) :
    timerManager(timerManager_), music(backend_, timerManager_) {}

  virtual void stopMusic() override;
  virtual void playTitleMusic() override;
  virtual void playGeneralStoreMusic() override;
  virtual void beep() override;
  virtual void honk() override;
};

}
}

#endif // TUNNELS_CLASSIC_SOUNDENGINE_H_
