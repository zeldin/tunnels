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
  if (database->getCurrentLocation() == LOCATION_ENTRANCE)
    lastActionKey = database->getKeymapEntry(KEYMAP_BREAK_DOOR);
  else  {
    // FIXME G@>658F
  }
  StartPosition pos = STARTPOS_NORMAL;
  if (database->getCurrentLocation() == LOCATION_ROOM ||
      database->getCurrentLocation() > LOCATION_FOUNTAIN) {
    if (lastActionKey != database->getKeymapEntry(KEYMAP_BREAK_DOOR))
      pos = STARTPOS_IN_DOORWAY;
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
  if (lastActionKey == 'C') // Not using keymap! G@>65A2
    database->setSecretDoorsRevealed(false);
  lastActionKey = 0;
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
  /* FIXME: If no direction set previous direction */
  screen.corridorScreen();
  for (;;) {
    MapPosition pos = database->getMapPosition();
    Location loc = database->mapLocation(pos);
    database->setCurrentLocation(loc);
    if (loc == LOCATION_ROOM || loc > LOCATION_FOUNTAIN)
      return DIVERSION_CONTINUE_GAME;
    screen.clearMessages();
    /* FIXME: V@>111D */
    database->setMapVisited(pos, true);
    for (unsigned n = 0; ;) {
      screen.drawCorridorSegment(n, loc);
      if (n != 0 && loc != LOCATION_CORRIDOR && loc != LOCATION_FOUNTAIN)
	break;
      if (n == 4)
	break;
      Location loc2;
      if (database->canMove(pos, left(direction), loc2)) {
	if (loc == LOCATION_ROOM && n == 0 &&
	    (loc2 == LOCATION_CORRIDOR || loc2 == LOCATION_FOUNTAIN))
	  loc2 = LOCATION_ROOM;
	else if (loc == LOCATION_FOUNTAIN &&
		 loc2 != LOCATION_BLANK && loc2 != LOCATION_FOUNTAIN)
	  loc2 = LOCATION_CORRIDOR;
	screen.drawCorridorLeftJunction(n, loc2);
      } else
	screen.drawCorridorLeftWall(n);
      if (database->canMove(pos, right(direction), loc2)) {
	if (loc == LOCATION_ROOM && n == 0 &&
	    (loc2 == LOCATION_CORRIDOR || loc2 == LOCATION_FOUNTAIN))
	  loc2 = LOCATION_ROOM;
	else if (loc == LOCATION_FOUNTAIN &&
		 loc2 != LOCATION_BLANK && loc2 != LOCATION_FOUNTAIN)
	  loc2 = LOCATION_CORRIDOR;
	screen.drawCorridorRightJunction(n, loc2);
      } else
	screen.drawCorridorRightWall(n);
      n++;
      if (!database->canMove(pos, direction, loc)) {
	loc = LOCATION_BLANK;
	continue;
      } else if(loc == LOCATION_ROOM)
	continue;
      pos.forward(direction);
      loc = database->mapLocation(pos);
      if ((loc == LOCATION_CORRIDOR || loc == LOCATION_FOUNTAIN) && n != 4)
	database->setMapVisited(pos, true);
    }
    screen.showCompass(direction);
    if (database->getCurrentLocation() == LOCATION_FOUNTAIN) {
      /* FIXME */
    }
    /* FIXME: V@>1D02 */
    backTarget = DIVERSION_CORRIDOR;
    /* FIXME: Advance time, clear stuff */
    byte keyCode;
    Direction dir;
    if (Diversion d = getMovementKey(keyCode, dir))
      return d;
    if (dir == DIR_NONE) {
      if (keyCode == database->getKeymapEntry(KEYMAP_CHECK_HIDDEN_DOORS)) {
	lastActionKey = keyCode;
	/* FIXME G@>67CE */
      }
      if (keyCode == database->getKeymapEntry(KEYMAP_BREAK_DOOR))
	dir = DIR_NORTH;
    }
    if (dir != DIR_NONE) {
      dir = direction + dir;
      if (dir != direction) {
	direction = dir;
	/* FIXME: G@>A028 */
	continue;
      }
      /* FIXME */
      if (keyCode == database->getKeymapEntry(KEYMAP_BREAK_DOOR))
	lastActionKey = database->getKeymapEntry(KEYMAP_BREAK_DOOR);
      MapPosition pos = database->getMapPosition();
      if (database->canMove(pos, direction, loc) &&
	  (loc == LOCATION_CORRIDOR || loc == LOCATION_FOUNTAIN)) {
	pos.forward(direction);
	database->setMapPosition(pos);
      }
      continue;
    } else if (keyCode == database->getKeymapEntry(KEYMAP_USE_ITEM)) {
      /* FIXME G@>67DF */
    } else if (keyCode == database->getKeymapEntry(KEYMAP_CHANGE_ORDER)) {
      /* FIXME G@>67E8 */
    } else if (keyCode == database->getKeymapEntry(KEYMAP_LISTEN_AT_DOOR)) {
      /* FIXME G@>67F2 */
    } else if (keyCode == database->getKeymapEntry(KEYMAP_TRADE_ITEMS)) {
      /* FIXME G@>6820 */
    } else {
      /* FIXME G@>682C */
    }

    return DIVERSION_NULL;
  }
}

GameEngine::Diversion GameEngine::core()
{
  database->setMapVisited(database->getMapPosition(), true);
  if (database->inCombat())
    return room();
  else switch(database->getCurrentLocation()) {
    case LOCATION_CORRIDOR:
    case LOCATION_FOUNTAIN:
      return DIVERSION_CORRIDOR;
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
