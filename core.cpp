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
  Database::StartPosition pos = Database::STARTPOS_NORMAL;
  if (database->getCurrentLocation() == Database::LOCATION_ROOM ||
      database->getCurrentLocation() > Database::LOCATION_FOUNTAIN) {
    // FIXME: In doorway...
  } else {
    Database::Location nextLocation;
    if (!database->canMove(database->getMapPosition(),
			   Database::reverse(static_cast<Database::Direction>(direction)),
			   nextLocation) ||
	(nextLocation != Database::LOCATION_CORRIDOR &&
	 nextLocation != Database::LOCATION_FOUNTAIN))
      pos = Database::STARTPOS_BACK_AGAINST_WALL;
  }
  database->setCurrentPlayer(-1);
  while (database->nextPlayerInOrder()) {
    unsigned p = database->getCurrentPlayer();
    database->setPlayerStartPosition(p, pos, static_cast<Database::Direction>(direction));
    screen.drawPlayer(p);
  }
  switch (database->getCurrentLocation()) {
  case Database::LOCATION_ENTRANCE:
    screen.drawGeneralStore();
  case Database::LOCATION_DESCENDING_STAIRCASE:
  case Database::LOCATION_ASCENDING_STAIRCASE:
    screen.drawStaircase();
    break;
  }

  if (database->getCurrentLocation() == Database::LOCATION_ENTRANCE &&
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
    case Database::LOCATION_CORRIDOR:
    case Database::LOCATION_FOUNTAIN:
      // -> G@>66F7
      return DIVERSION_NULL;
    case Database::LOCATION_ROOM:
      /* FIXME: Find current room descriptor */
    case Database::LOCATION_DESCENDING_STAIRCASE:
    case Database::LOCATION_ASCENDING_STAIRCASE:
    case Database::LOCATION_ENTRANCE:
      return room();
    default:
      return DIVERSION_QUIT;
    }
}

}
