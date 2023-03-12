#ifndef TUNNELS_ENGINE_H_
#define TUNNELS_ENGINE_H_

namespace Tunnels {

class ScreenEngine {
public:
  virtual ~ScreenEngine() {}
  virtual void refresh() {}
  virtual void drawTitleScreen() = 0;
};

class SoundEngine {
public:
  virtual ~SoundEngine() {}
  virtual void stopMusic() = 0;
  virtual void playTitleMusic() = 0;
};

}

#endif // TUNNELS_ENGINE_H_
