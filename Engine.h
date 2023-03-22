#ifndef TUNNELS_ENGINE_H_
#define TUNNELS_ENGINE_H_

#include "EventLoop.h"

namespace Tunnels {

namespace Timer { class TimerManager; }

class ScreenEngine {
public:
  virtual ~ScreenEngine() {}
  virtual void refresh() {}
  virtual void drawTitleScreen() = 0;
  virtual void drawPrompt(unsigned n) = 0;
};

class SoundEngine {
public:
  virtual ~SoundEngine() {}
  virtual void stopMusic() = 0;
  virtual void playTitleMusic() = 0;
  virtual void beep() = 0;
  virtual void honk() = 0;
};

class GameEngine {
private:
  EventLoop &eventLoop;
  Timer::TimerManager &timerManager;
  ScreenEngine &screen;
  SoundEngine &sound;

public:
  GameEngine(EventLoop &eventLoop_, Timer::TimerManager &timerManager_,
	     ScreenEngine &screen_, SoundEngine &sound_);
  EventType run();

private:
  Event nextEvent();
  EventType waitForEvent();
  EventType titleScreen();
};

}

#endif // TUNNELS_ENGINE_H_
