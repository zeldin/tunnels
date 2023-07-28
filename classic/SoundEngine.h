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

  byte vaultSound[8];

public:
  SoundEngine(DCSG::Backend &backend_, Timer::TimerManager &timerManager_);

  virtual void stopMusic() override;
  virtual void playTitleMusic() override;
  virtual void playStairMusic() override;
  virtual void playGeneralStoreMusic() override;
  virtual void playMonsterSound(unsigned n) override;
  virtual void playQuestObjectFailedMusic() override;
  virtual void playQuestObjectCompleteMusic() override;
  virtual void playVaultCombinationFoundMusic() override;
  virtual void playVaultDigitSound(unsigned n) override;
  virtual void beep() override;
  virtual void honk() override;
};

}
}

#endif // TUNNELS_CLASSIC_SOUNDENGINE_H_
