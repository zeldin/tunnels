#include "system.h"

#include "Engine.h"
#include "Utils.h"
#include "Database.h"

namespace Tunnels {

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

  waitForEvent();
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
      // -> G@>66F7
      return DIVERSION_NULL;
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
