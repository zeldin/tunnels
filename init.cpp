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

GameEngine::Checkpoint GameEngine::titleScreen()
{
  screen.drawTitleScreen();
  sound.playTitleMusic();
  EventType e = waitForEvent();
  sound.stopMusic();
  timerManager.removeTimeout();
  return e == EVENT_QUIT? CHECKPOINT_QUIT : CHECKPOINT_LOAD_SAVE;
}

EventType GameEngine::run()
{
  Checkpoint checkpoint = titleScreen();
  progression = 2;
  for (;;)
    switch (checkpoint) {
    case CHECKPOINT_LOAD_SAVE:
      if ((checkpoint = loadSave()))
	continue;
    case CHECKPOINT_NEW_OR_RESTOCK:
    default:
      // internal error...
      return EVENT_NULL;
    case CHECKPOINT_QUIT:
      return EVENT_QUIT;
    }
}

}
