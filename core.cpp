#include "system.h"

#include "Engine.h"
#include "Utils.h"
#include "Database.h"

namespace Tunnels {

bool GameEngine::tryMove()
{
  // G@A028
  return false;
}

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
    screen.drawPrompt(0x6a);
    /* ... */
  }
  if (kc == '2') {
    screen.drawPrompt(0x6d);
    acceptMask = ACCEPT_BACK;
    for (;;)
      if (Diversion d = getKeyNoCursor(kc))
	return d;
  }
  if (kc == '1') {
    bool items = false;
    int player = database->getCurrentPlayer();
    for (;;) {
      if (player < 0)
	player = database->getNumConfiguredPlayers()-1;
      if (player >= database->getNumConfiguredPlayers())
	player = 0;
      screen.drawPrompt(0x27); // ?!
      screen.drawPrompt(0x10);
      screen.drawPlayerStatusHeader(player);
      if (!items) {
	screen.drawPrompt(0x6b);
	do {
	  acceptMask = ACCEPT_BACK | ACCEPT_UP_DOWN | ACCEPT_ALPHANUMERIC;
	  if (Diversion d = getKeyNoCursor(kc))
	    return d;
	} while (kc > KEY_BACK &&
		 kc != database->getKeymapEntry(KEYMAP_CHANGE_WEAPON));
	if (kc == database->getKeymapEntry(KEYMAP_CHANGE_WEAPON) &&
	    !database->inCombat())
	  database->swapPlayerWeapon(player);
      } else {
	database->compactPlayerMagicItems(player);
	screen.drawPrompt(0x6c);
	screen.prepareItemNumberInput();
	acceptMask = ACCEPT_BACK | ACCEPT_UP_DOWN | ACCEPT_NUMERIC;
	unsigned itemNumber;
	if (Diversion d = getNumber(1, 10, itemNumber))
	  switch(d) {
	  case DIVERSION_UP:
	    kc = KEY_UP;
	    break;
	  case DIVERSION_DOWN:
	    kc = KEY_DOWN;
	    break;
	  default:
	    return d;
	  }
	else {
	  --itemNumber;
	  byte id = database->getPlayerMagicItemId(player, itemNumber);
	  if (int8(id) <= 0 ||
	      !database->getPlayerMagicItemRemainingUses(player, itemNumber))
	    sound.honk();
	  else {
	    screen.drawMagicItemDescription(id);
	    if ((d = flashBorder()))
	      return d;
	  }
	  continue;
	}
      }
      if (kc == KEY_UP) {
	if (!(items = !items))
	  player++;
      }
      if (kc == KEY_DOWN) {
	if ((items = !items))
	  --player;
      }
    }
  }
  if (kc == 'E') dir = DIR_NORTH;
  if (kc == 'D') dir = DIR_EAST;
  if (kc == 'X') dir = DIR_SOUTH;
  if (kc == 'S') dir = DIR_WEST;
  if (kc == database->getKeymapEntry(KEYMAP_CHECK_HIDDEN_DOORS)) {
    database->setSecretDoorsRevealed(true);
  }
  if (kc == database->getKeymapEntry(KEYMAP_SAVE_GAME))
    return DIVERSION_LOAD_SAVE_BACK;
  if (kc == database->getKeymapEntry(KEYMAP_LOOK_AT_MAP)
      && database->getCurrentLocation() != LOCATION_ENTRANCE) {
    screen.mapScreen();
    acceptMask = ACCEPT_BACK;
    for (;;)
      if (Diversion d = getKeyNoCursor(kc))
	return d;
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

  database->setCurrentPlayer(-1);

  if (database->getCurrentLocation() == LOCATION_ENTRANCE &&
      database->getPartyGold() != 0) {
    sound.playGeneralStoreMusic();
    if (Diversion d = waitForMusic())
      return d;
    // ...
  }

  for (;;) {
    backTarget = DIVERSION_CONTINUE_GAME;
    // ...
    screen.drawPrompt(0x2e);
    byte keyCode;
    Direction dir;
    if (Diversion d = getMovementKey(keyCode, dir))
      return d;
    if (dir != DIR_NONE) {
      direction = dir;
      if (database->getCurrentLocation() == LOCATION_ENTRANCE ||
	  !tryMove())
	continue;
    } else {
      if (keyCode == database->getKeymapEntry(KEYMAP_USE_ITEM)) {
	/* ... */
      }
      if (keyCode == database->getKeymapEntry(KEYMAP_CHANGE_ORDER)) {
	/* ... */
      }
      if (keyCode == database->getKeymapEntry(KEYMAP_TRADE_ITEMS)) {
	/* ... */
      }
      if (keyCode == KEY_UP) {
	/* ... */
      }
      if (keyCode == KEY_DOWN) {
	/* ... */
      }
    }
    return DIVERSION_CONTINUE_GAME;
  }
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
