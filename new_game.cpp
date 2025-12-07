#include "system.h"

#include "Engine.h"
#include "EventLoop.h"
#include "Database.h"
#include "Utils.h"

namespace Tunnels {

GameEngine::Diversion GameEngine::digFloor()
{
  database->digFloor();
  return delay(500);
}

GameEngine::Diversion GameEngine::stockFloor()
{
  database->stockFloor();
  return delay(250);
}

GameEngine::Diversion GameEngine::buildDungeon()
{
  progression = 9;
  // FIXME: floor room descriptors = &descriptor 0   G@>62F7
  database->setNumStockedFloors(0);
  database->setCurrentPlayer(-1);
  database->setCurrentFloor(1);
  database->setPartyGold(0);
  database->initPlayerOrder();
  while (database->nextPlayerInOrder()) {
    unsigned player = database->getCurrentPlayer();
    unsigned c = database->getPlayerClass(player);
    database->clearPlayerSheet(player);
    database->setPlayerClass(player, c);
    database->setPlayerWeaponDamage(player, false, database->getDefaultWeaponDamage());
    database->setPlayerWeaponDamage(player, true, database->getDefaultWeaponDamage());
    byte gold = database->getClassInitialGold(c);
    switch (database->getDifficulty()) {
    case 0: break;
    case 1: gold -= gold >> 2; break;
    default: gold -= gold >> 1; break;
    }
    database->setPartyGold(database->getPartyGold() + gold);
    for (unsigned n=0;; ) {
      ItemCategory cat;
      bool secondary, inventoryFull;
      unsigned slot;
      if (byte id = database->getClassInitialEquipment(c, n, cat))
	switch(cat) {
	case ITEM_ARMORS:
	  takeArmor(player, id, inventoryFull);
	  break;
	case ITEM_SHIELDS:
	  takeShield(player, id, inventoryFull);
	  break;
	case ITEM_WEAPONS:
	case ITEM_RANGED_WEAPONS:
	  takeWeapon(player, cat, id, secondary, inventoryFull);
	  break;
	case ITEM_MAGIC_ITEMS:
	  takeMagicItem(player, id, slot);
	  break;
	}
      else
	break;
    }
  }
  screen.drawPrompt(0x24);
  sound.playTitleMusic();
  do {
    byte kc;
    if (Diversion d = rawGetKey(kc, true))
      return d;
    else if (kc == KEY_BACK)
      return DIVERSION_NEW_OR_RESTOCK;
    if (!database->getNumDugFloors()) {
      screen.drawPrompt(0x25);
      if (Diversion d = digFloor())
	return d;
    }
    screen.drawPrompt(0x26);
    if (Diversion d = stockFloor())
      return d;
    // FIXME: advance floor room descriptors  G@>63ED
    database->setCurrentFloor(database->getCurrentFloor() + 1);
  } while(database->getCurrentFloor() <= database->getNumFloors());
  database->setNumDugFloors(database->getNumFloors());
  database->setNumStockedFloors(database->getNumFloors());
  progression = 10;
  database->setMapPosition(database->getEntryStairsPosition());
  database->setUnknown10FA(0);
  // Weird write to @>8338 at G@>6421 skipped...
  database->setCurrentFloor(0);
  database->setUnknown1CF9(0);
  direction = DIR_NORTH;
  database->setCurrentPlayer(0);
  database->setUnknown10FB(2);
  database->setMappedFloors(0);
  database->setUnknown10FF(0);
  database->resetQuestObjects();
  database->resetRationTurns();
  database->resetUnknown1118();
  database->resetHealingTurns();
  database->resetSubtime();
  database->resetUnknown111C();
  database->resetUnknown111D();
  database->clearCombat();
  database->resetEncounterProbability();
  database->resetRationInterval();
  database->setInitialRations();
  database->resetHealingInterval();
  database->setCurrentMagicalEffect(TIMED_MAGICAL_EFFECT_SPEED, 0);
  database->setCurrentMagicalEffect(TIMED_MAGICAL_EFFECT_ENCOUNTER_PROBABILITY, 0);
  database->setCurrentMagicalEffect(TIMED_MAGICAL_EFFECT_RATION_INTERVAL, 0);
  database->setCurrentMagicalEffect(TIMED_MAGICAL_EFFECT_HEALING_INTERVAL, 0);
  if (byte startFloor = database->getStartFloor()) {
    database->setCurrentLocation(LOCATION_BLANK);
    screen.stairMovement(false);
    database->setCurrentFloor(startFloor);
    MapPosition pos;
    // Hmm, original code at G@>64E8 actually uses current
    // value of @>8336 for the room descriptor, which at this
    // point would point to just _after_ the last generated floor, since
    // G@>A03E has not been called yet (that happens at G@>6500).
    // Also unclear why it uses the first normal room instead of the
    // first ascending staircase room
    currentRoom = database->getFirstRoomDescriptor(pos);
    database->setMapPosition(pos);
    return DIVERSION_STAIRCASE;
  } else
    return DIVERSION_ENTRANCE;
}

GameEngine::Diversion GameEngine::newOrRestockMenu()
{
  redoTarget = DIVERSION_NEW_OR_RESTOCK;
  backTarget = DIVERSION_LOAD_SAVE_BACK;
  progression = 4;
  acceptMask = ACCEPT_BACK | ACCEPT_REDO | ACCEPT_NUMERIC;
  byte dugFloors = database->getNumDugFloors();
  if ((dugFloors & 0x80))
    database->setNumDugFloors(dugFloors = -dugFloors); // Clear re-dig flag
  byte x = 0;
  Diversion d;
  if (dugFloors) {
    screen.drawPrompt(0x1a);
    if (database->getNumStockedFloors() == 0)
      d = getNumberKey(x, 1, 2);
    else {
      // Can continue
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
    database->setNumDugFloors(dugFloors = -dugFloors); // Set re-dig flag
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
  if ((dugFloors & 0x80))
    // Discard old dug dungeon
    database->setNumDugFloors(0);
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
  database->setNumStockedFloors(0); // Prevent continue game
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
