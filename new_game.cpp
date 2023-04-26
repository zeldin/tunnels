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
    if ((d = delay(667)))
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
  if ((d = delay(667)))
    return d;
  if (database->getNumConfiguredPlayers() == database->getNumPlayers())
    return DIVERSION_POINT_OF_NR;
  else
    return DIVERSION_CREATE_PARTY;
}

GameEngine::Diversion GameEngine::createPartyMember(unsigned player)
{
  database->clearPlayerSheet(player);
  screen.drawPrompt(0x21);
  acceptMask = ACCEPT_BACK | ACCEPT_REDO | ACCEPT_ALPHANUMERIC;
  screen.preparePlayerNameInput(player);
  for (;;) {
    byte name[15];
    Diversion d = getString(name);
    if (d) return d;
    if (name[0] != ' ') {
      database->setPlayerName(player, name);
      break;
    }
    sound.honk();
  }
  unsigned classId, classChoices = database->getNumClassChoices();
  acceptMask = ACCEPT_BACK | ACCEPT_REDO | ACCEPT_NUMERIC; // BACK was PROC'D...
  if (classChoices > 1) {
    screen.preparePlayerClassInput();
    Diversion d = getNumber(1, classChoices, classId);
    if (d) return d;
    --classId;
  } else
    classId = 0;
  database->setPlayerClass(player, classId);
  screen.setPlayerShapes(player);
  for (;;) {
    unsigned color;
    screen.preparePlayerColorInput();
    Diversion d = getNumber(1, 4, color);
    if (d) return d;
    database->setPlayerColor(player, color-1);
    unsigned p;
    for (p=0; p<player; p++)
      if (database->getPlayerColor(p) == database->getPlayerColor(player) &&
	  database->getPlayerClass(p) == database->getPlayerClass(player))
	break;
    if (p >= player)
      break;
    sound.honk();
    screen.drawPrompt(0x22);
  }
  procdTarget = DIVERSION_PROCD;
  screen.askCharacterAccept();
  acceptMask = ACCEPT_PROCD | ACCEPT_REDO | ACCEPT_BACK;
  byte kc;
  return getKeyNoCursor(kc);
}

GameEngine::Diversion GameEngine::createPartyMenu()
{
  database->setNumConfiguredPlayers(0);
  database->setUnknown1CEB(0);
  backTarget = DIVERSION_NEW_OR_RESTOCK;
  redoTarget = DIVERSION_REDO;
  unsigned numPlayers = database->getNumPlayers(), player = 0;
  while (player < numPlayers) {
    Diversion d = createPartyMember(player);
    if (d == DIVERSION_PROCD)
      d = DIVERSION_NULL;
    else if (d == DIVERSION_REDO)
      continue;
    else if (d)
      return d;
    player++;
  }
  return DIVERSION_NULL;
}

GameEngine::Diversion GameEngine::pointOfNoReturnMenu()
{
  screen.setPlayerColors();
  database->setNumConfiguredPlayers(database->getNumPlayers());
  screen.drawPrompt(0x23);
  redoTarget = DIVERSION_CREATE_PARTY;
  procdTarget = DIVERSION_BUILD_DUNGEON;
  acceptMask = ACCEPT_PROCD | ACCEPT_REDO;
  byte kc;
  return getKeyNoCursor(kc);
}

}
