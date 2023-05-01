#include "system.h"

#include "Engine.h"
#include "Utils.h"
#include "Database.h"

namespace Tunnels {

GameEngine::Diversion GameEngine::getMovementKey(byte &kc, Direction &dir)
{
  acceptMask =
    ACCEPT_AID | ACCEPT_UP_DOWN | ACCEPT_NUMERIC | ACCEPT_ALPHANUMERIC;
  dir = DIR_NONE;
  if (Diversion d = getKeyNoCursor(kc))
    return d;
  if (kc == KEY_AID) {
    int page = 0;
    for (;;) {
      screen.drawPrompt(0x27);
      if (page < 0)
	page = 2;
      if (page > 2)
	page = 0;
      switch (page) {
      case 0: screen.drawPrompt(0x65); break;
      case 1: screen.drawPrompt(0x67); break;
      case 2: screen.drawPrompt(0x68); break;
      }
      acceptMask = ACCEPT_BACK | ACCEPT_UP_DOWN;
      if (Diversion d = getKeyNoCursor(kc))
	return d;
      if (kc == KEY_DOWN)
	--page;
      else
	page++;
    }
  }
  if (kc == '3' && database->inCombat()) {
    /* ... */
  }
  if (kc == '2') {
    /* ... */
  }
  if (kc == '1') {
    /* ... */
  }
  if (kc == 'E') dir = DIR_NORTH;
  if (kc == 'D') dir = DIR_EAST;
  if (kc == 'X') dir = DIR_SOUTH;
  if (kc == 'S') dir = DIR_WEST;
  if (kc == 'C') {
    /* ... */
  }
  if (kc == 'K')
    return DIVERSION_LOAD_SAVE_BACK;
  if (kc == 'M') {
    /* ... */
  }
  return DIVERSION_NULL;
}

GameEngine::Diversion GameEngine::room()
{
  // G@>657E
  sound.stopMusic();
  screen.roomScreen();
  StartPosition pos = STARTPOS_NORMAL;
  if (database->getCurrentLocation() == LOCATION_ROOM ||
      database->getCurrentLocation() > LOCATION_FOUNTAIN) {
    // FIXME: In doorway...
  } else {
    Location nextLocation;
    if (!database->canMove(database->getMapPosition(), reverse(direction),
			   nextLocation) ||
	(nextLocation != LOCATION_CORRIDOR &&
	 nextLocation != LOCATION_FOUNTAIN))
      pos = STARTPOS_BACK_AGAINST_WALL;
  }
  database->setCurrentPlayer(-1);
  while (database->nextPlayerInOrder()) {
    unsigned p = database->getCurrentPlayer();
    database->setPlayerStartPosition(p, pos, direction);
    screen.drawPlayer(p);
  }
  switch (database->getCurrentLocation()) {
  case LOCATION_ENTRANCE:
    screen.drawGeneralStore();
  case LOCATION_DESCENDING_STAIRCASE:
  case LOCATION_ASCENDING_STAIRCASE:
    screen.drawStaircase();
    break;
  }

  if (database->getCurrentLocation() == LOCATION_ENTRANCE &&
      database->getPartyGold() != 0) {
    sound.playGeneralStoreMusic();
    if (Diversion d = waitForMusic())
      return d;
    // ...
  }

  backTarget = DIVERSION_CONTINUE_GAME;
  // ...
  screen.drawPrompt(0x2e);
  byte keyCode;
  Direction dir;
  if (Diversion d = getMovementKey(keyCode, dir))
    return d;
  if (dir != DIR_NONE) {
    /* ... */
  } else {
    /* ... */
  }
  return DIVERSION_NULL;
}

GameEngine::Diversion GameEngine::corridor()
{
  // -> G@>66F7
  return DIVERSION_NULL;
}

GameEngine::Diversion GameEngine::core()
{
  database->setMapVisited(database->getMapPosition(), true);
  if (database->inCombat())
    return room();
  else switch(database->getCurrentLocation()) {
    case LOCATION_CORRIDOR:
    case LOCATION_FOUNTAIN:
      return corridor();
    case LOCATION_ROOM:
      /* FIXME: Find current room descriptor */
    case LOCATION_DESCENDING_STAIRCASE:
    case LOCATION_ASCENDING_STAIRCASE:
    case LOCATION_ENTRANCE:
      return room();
    default:
      return DIVERSION_QUIT;
    }
}

}
