#include "system.h"

#include "Engine.h"
#include "EventLoop.h"
#include "Timer.h"

namespace Tunnels {

GameEngine::GameEngine(EventLoop &eventLoop_,
		       Timer::TimerManager &timerManager_,
		       ScreenEngine &screen_, SoundEngine &sound_)
  : eventLoop(eventLoop_), timerManager(timerManager_),
    screen(screen_), sound(sound_)
{
}

Event GameEngine::nextEvent()
{
  screen.refresh();
  return eventLoop.runEventLoop(timerManager);
}

EventType GameEngine::waitForEvent()
{
  return nextEvent().type();
}

GameEngine::Diversion GameEngine::titleScreen()
{
  screen.drawTitleScreen();
  sound.playTitleMusic();
  EventType e = waitForEvent();
  sound.stopMusic();
  timerManager.removeTimeout();
  return e == EVENT_QUIT? DIVERSION_QUIT : DIVERSION_LOAD_SAVE;
}

EventType GameEngine::run()
{
  Diversion diversion = titleScreen();
  progression = 2;
  for (;;)
    switch (diversion) {
    case DIVERSION_LOAD_SAVE:
      if ((diversion = loadSave()))
	continue;
    case DIVERSION_NEW_OR_RESTOCK:
    default:
      // internal error...
      return EVENT_NULL;
    case DIVERSION_QUIT:
      return EVENT_QUIT;
    }
}

}
