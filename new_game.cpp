#include "system.h"

#include "Engine.h"
#include "EventLoop.h"
#include "Database.h"
#include "Utils.h"

namespace Tunnels {

GameEngine::Diversion GameEngine::newOrRestockMenu()
{
  redoTarget = DIVERSION_NEW_OR_RESTOCK;
  backTarget = DIVERSION_LOAD_SAVE_BACK;
  progression = 4;
  acceptMask = ACCEPT_BACK | ACCEPT_REDO | ACCEPT_NUMERIC;
  byte unknown = database->getUnknown1CF4();
  if ((unknown & 0x80))
    database->setUnknown1CF4(unknown = -unknown);
  byte x = 0;
  Diversion d;
  if (unknown) {
    screen.drawPrompt(0x1a);
    if (database->getUnknown1CEB() == 0)
      d = getNumberKey(x, 1, 2);
    else {
      screen.drawPrompt(0x1b);
      d = getNumberKey(x, 1, 3);
    }
    if (d)
      return d;
    if ((d = delay(3600)))
      return d;
    backTarget = DIVERSION_NEW_OR_RESTOCK;
  }
  if (x == 2)
    return DIVERSION_CONTINUE_GAME;
  if (x == 0) {
    database->setUnknown1CF4(unknown = -unknown);
    screen.drawPrompt(0x0e);
    if (database->getMinFloors() == database->getMaxFloors())
      database->setNumFloors(database->getMinFloors());
    else {
      screen.drawPrompt(0x1d);
      unsigned floors;
      if ((d = getNumber(database->getMinFloors(), database->getMaxFloors(),
			 floors)))
	return d;
      database->setNumFloors(floors);
    }
  } else {
    screen.drawPrompt(0x0e);
  }
  if (database->getMaxPlayers() == 1)
    database->setNumPlayers(1);
  else {
    screen.drawPrompt(0x1e);
    unsigned players;
    if ((d = getNumber(1, database->getMaxPlayers(), players)))
      return d;
    database->setNumPlayers(players);
  }
  screen.drawPrompt(0x1c);
  if ((d = getNumberKey(x, 1, 3)))
    return d;
  database->setDifficulty(x);
  if ((unknown & 0x80))
    database->setUnknown1CF4(0);
  if ((d = delay(3600)))
    return d;
  // 61D4
  return DIVERSION_NULL;
}

}
