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
  // ...
  switch (database->getCurrentLocation()) {
  case Database::LOCATION_ENTRANCE:
    screen.drawGeneralStore();
  case Database::LOCATION_DESCENDING_STAIRCASE:
  case Database::LOCATION_ASCENDING_STAIRCASE:
    screen.drawStaircase();
    break;
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
