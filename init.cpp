#include "system.h"

#include "Engine.h"
#include "EventLoop.h"
#include "Timer.h"

namespace Tunnels {

GameEngine::GameEngine(EventLoop &eventLoop_,
		       Timer::TimerManager &timerManager_,
		       ScreenEngine &screen_, SoundEngine &sound_,
		       File::Backend &file_, DatabaseFactory &databaseFactory_)
  : eventLoop(eventLoop_), timerManager(timerManager_),
    screen(screen_), sound(sound_), file(file_),
    databaseFactory(databaseFactory_), database(nullptr)
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

GameEngine::Diversion GameEngine::delay(unsigned ms)
{
  timerManager.setTimeoutAfter(ms);
  EventType e;
  do {
    e = waitForEvent();
  } while(e != EVENT_TIMEOUT && e != EVENT_QUIT);
  timerManager.removeTimeout();
  return (e == EVENT_QUIT? DIVERSION_QUIT : DIVERSION_NULL);
}

GameEngine::Diversion GameEngine::flashBorder()
{
  EventType e;
  screen.setBorderFlashEnabled(true);
  do {
    e = waitForEvent();
  } while(e != EVENT_KEY && e != EVENT_QUIT);
  screen.setBorderFlashEnabled(false);
  return (e == EVENT_QUIT? DIVERSION_QUIT : DIVERSION_NULL);
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
    case DIVERSION_LOAD_SAVE_BACK:
      progression = 3;
    case DIVERSION_LOAD_SAVE:
      if ((diversion = loadSaveMenu()))
	continue;
    case DIVERSION_NEW_OR_RESTOCK:
      if ((diversion = newOrRestockMenu()))
	continue;
    default:
      // internal error...
      return EVENT_NULL;
    case DIVERSION_QUIT:
      return EVENT_QUIT;
    }
}

}
