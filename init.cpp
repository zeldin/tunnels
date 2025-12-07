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
    databaseFactory(databaseFactory_), database(nullptr), randState(0x3567),
    roomDone(0)
{
}

byte GameEngine::random(byte upper)
{
  // G@>027A
  uint16 n = 0x6fe5 * randState + 0x7ab9;
  randState = n;
  n = (n >> 8) | (n << 8);
  return n % (upper + 1);
}

byte GameEngine::random(byte lower, byte upper)
{
  // G@>A3E6
  if (upper <= lower)
    return upper;
  byte n;
  do {
    n = random(upper > 60? 125 :
	       (upper > 30? 60 :
		(upper > 15? 30 :
		 upper > 5? 15 : 5)));
  } while(n < lower || n > upper);
  return n;
}

Event GameEngine::nextEvent(bool nonBlock)
{
  screen.refresh();
  return eventLoop.runEventLoop(timerManager, nonBlock);
}

EventType GameEngine::waitForEvent()
{
  return nextEvent(false).type();
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

GameEngine::Diversion GameEngine::waitForMusic()
{
  for (;;)
    switch (waitForEvent()) {
    case EVENT_QUIT: return DIVERSION_QUIT;
    case EVENT_END_OF_MUSIC: return DIVERSION_NULL;
    }
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
    case DIVERSION_CREATE_PARTY:
      if ((diversion = createPartyMenu()))
	continue;
    case DIVERSION_POINT_OF_NR:
      if ((diversion = pointOfNoReturnMenu()))
	continue;
    case DIVERSION_BUILD_DUNGEON:
      if ((diversion = buildDungeon()))
	continue;
    default:
      // internal error...
      return EVENT_NULL;
    case DIVERSION_STAIRCASE:
      diversion = staircase();
      continue;
    case DIVERSION_ENTRANCE:
      diversion = entrance();
      continue;
    case DIVERSION_ENTER_LOCATION:
    case DIVERSION_CONTINUE_GAME:
      diversion = core(diversion == DIVERSION_ENTER_LOCATION);
      continue;
    case DIVERSION_ENTER_ROOM:
    case DIVERSION_ROOM_SETUP:
      roomSetup(diversion == DIVERSION_ENTER_ROOM);
    case DIVERSION_ROOM_MAIN:
      diversion = room();
      continue;
    case DIVERSION_CORRIDOR:
      // -> G@>66F7
      /* FIXME: If no direction set previous direction */
      screen.corridorScreen();
    case DIVERSION_CORRIDOR_MAIN:
      diversion = corridor();
      continue;
    case DIVERSION_GENERAL_STORE:
      diversion = generalStore();
      continue;
    case DIVERSION_TRADE_ITEM:
      diversion = tradeItem(currentItemCategory, currentItem, currentItemStat, currentItemAmmo);
      continue;
    case DIVERSION_GIVE_ITEM:
      diversion = giveItem(currentItemCategory, currentItem, currentItemStat, currentItemAmmo);
      continue;
    case DIVERSION_PARTY_ORDER:
      diversion = partyOrder();
      continue;
    case DIVERSION_VAULT:
      diversion = vault();
      continue;
    case DIVERSION_LOOT_ROOM:
      diversion = lootRoom();
      continue;
    case DIVERSION_LOOT_FIRST_ITEM:
      itemIterPos = 0;
      diversion = lootItems();
      continue;
    case DIVERSION_LOOT_ITEM_DONE:
      if ((diversion = flashBorder()))
	continue;
    case DIVERSION_LOOT_NEXT_ITEM:
      roomSetup(false);
      diversion = lootItems();
      continue;
    case DIVERSION_LOOT_FIXTURES:
      diversion = lootFixtures();
      continue;
    case DIVERSION_QUIT:
      return EVENT_QUIT;
    }
}

}
