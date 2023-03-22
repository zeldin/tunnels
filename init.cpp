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

EventType GameEngine::titleScreen()
{
  screen.drawTitleScreen();
  sound.playTitleMusic();
  EventType e = waitForEvent();
  sound.stopMusic();
  timerManager.removeTimeout();
  return e;
}

EventType GameEngine::run()
{
  if (titleScreen() == EVENT_QUIT)
    return EVENT_QUIT;
  screen.drawPrompt(0x11);
  byte x;
  if (getNumberKey(x, 1, 3).type() == EVENT_QUIT)
    return EVENT_QUIT;
  waitForEvent();
  return EVENT_NULL;
}

}
