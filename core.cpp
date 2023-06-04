#include "system.h"

#include "Engine.h"
#include "Utils.h"
#include "Database.h"

namespace Tunnels {

void GameEngine::pickUnoccupiedRoomSquare(byte &y, byte &x)
{
  // G@>ADB5
  do {
    y = random(1, 6);
    x = random(1, 6);
  } while(screen.checkIfRoomSquareOccupied(y, x));
}

void GameEngine::pickItemRoomSquare(byte &y, byte &x)
{
  // G@>ADD5
  byte weight;
  do {
    pickUnoccupiedRoomSquare(y, x);
    // Make sure square is either in a corner or the center
    weight = (y > 3? 7 - y : y) + (x > 3? 7 - x : x);
    // 2 3 4 4 3 2
    // 3 4 5 5 4 3
    // 4 5 6 6 5 4
    // 4 5 6 6 5 4
    // 3 4 5 5 4 3
    // 2 3 4 4 3 2
  } while(weight >= 4 &&
	  (weight < 6 || database->roomHasLivingStatue(currentRoom) ||
	   database->roomHasFountain(currentRoom)));
}

void GameEngine::setRoomFixtureShape(RoomFixture f)
{
  database->setRoomFixture(f);
  screen.setRoomFixtureShapes();
}

bool GameEngine::tryMove(bool checkOnly)
{
  // G@A028
  // G@A9B5
  MapPosition pos = database->getMapPosition();
  Location loc;
  currentRoom = invalidHandle();
  if (!database->canMove(pos, direction, loc))
    return false;
  database->clearRoomFixtures();
  screen.setRoomFixtureShapes();
  database->clearRoomEnemies();
  switch (loc) {
  case LOCATION_ROOM:
    pos.forward(direction);
    currentRoom = database->getRoomDescriptor(pos);
    pos.backward(direction);
    {
      byte f = database->getRoomSpecialType(currentRoom);
      if (f & 0x80)
	f = -f;
      if (f) {
	setRoomFixtureShape(RoomFixture(FIXTURE_GENERAL_STORE + (f-1)));
	break;
      }
    }
    if (database->roomHasEnemies(currentRoom)) {
      database->prepareRoomEnemies(currentRoom);
      screen.setRoomFixtureShapes();
    }
    break;
  case LOCATION_FOUNTAIN:
    setRoomFixtureShape(FIXTURE_FOUNTAIN);
    break;
  case LOCATION_DESCENDING_STAIRCASE:
    setRoomFixtureShape(FIXTURE_DESCENDING_STAIRS);
    break;
  case LOCATION_ASCENDING_STAIRCASE:
    setRoomFixtureShape(FIXTURE_ASCENDING_STAIRS);
    break;
  }
  // G@>A76B
  if (checkOnly)
    return true;
  pos.forward(direction);
  database->setMapPosition(pos);
  loc = database->mapLocation(pos);
  database->setCurrentLocation(loc);
  for (RoomItem n = ROOM_ITEM_FIRST; n<=ROOM_ITEM_LAST; n=next(n))
    database->clearRoomItemPosition(n);
  database->setSecretDoorsRevealed(false);
  return true;
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
    acceptMask = ACCEPT_BACK;
    for (;;)
      if (Diversion d = getKeyNoCursor(kc))
	return d;
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

GameEngine::Diversion GameEngine::staircase()
{
  // G@>64F1
  if (database->getCurrentLocation() == LOCATION_ENTRANCE)
    return DIVERSION_ENTER_ROOM;
  lastActionKey = database->getKeymapEntry(KEYMAP_BREAK_DOOR);
  sound.playStairMusic();
  if (Diversion d = delay(2000))
    return d;
  database->prepareFloorMap(database->getCurrentFloor());
  database->restoreFloorVisitedMarkers();
  return DIVERSION_ENTER_LOCATION;
}

GameEngine::Diversion GameEngine::entrance()
{
  // G@>6538
  database->setCurrentLocation(LOCATION_ENTRANCE);
  lastActionKey = database->getKeymapEntry(KEYMAP_BREAK_DOOR);
  setRoomFixtureShape(FIXTURE_DESCENDING_STAIRS);
  return DIVERSION_ENTER_ROOM;
}

void GameEngine::placeRoomItems()
{
  // G@>AB2B
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
    if (database->getPlayerHP(p) > database->getPlayerWD(p))
      screen.drawPlayer(p);
  }
  switch (database->getCurrentLocation()) {
  case LOCATION_ROOM:
    switch(database->getRoomSpecialType(currentRoom)) {
    case 0:
      byte prevy[5], prevx[5];
      for (RoomItem i = ROOM_ITEM_FIRST; i <= ROOM_ITEM_LAST; i=next(i)) {
	byte y, x;
	bool collission;
	do {
	  pickItemRoomSquare(y, x);
	  collission = false;
	  for (unsigned j = 0; j < i; j++)
	    if (y == prevy[j] && x == prevx[j]) {
	      collission = true;
	      break;
	    }
	} while(collission);
	prevy[i] = y;
	prevx[i] = x;
	database->placeRoomItem(i, y, x);
      }
      if (database->roomHasLivingStatue(currentRoom)) {
	database->placeRoomItemCenter(ROOM_ITEM_CENTERPIECE);
	screen.drawLivingStatue();
      }
      if (database->roomHasFountain(currentRoom)) {
	database->placeRoomItemCenter(ROOM_ITEM_CENTERPIECE);
	screen.drawFountain();
      }
      drawLoot();
      break;
    case 1:
      // General store
      database->placeRoomItem(ROOM_ITEM_FIXTURE, 6, 6);
      screen.drawDynamicFixture();
      break;
    case 2:
      // Vault
      database->clearRoomItemPosition(ROOM_ITEM_FIXTURE);
      for (RoomItem n=ROOM_ITEM_MONEY; n<=ROOM_ITEM_LAST; n=next(n))
	database->placeRoomItem(n, 6, 1);
      return;
    default:
      return;
    }
  case LOCATION_CORRIDOR:
    // FIXME: G@>AC5A
    break;
  case LOCATION_ENTRANCE:
    screen.drawGeneralStore();
    setRoomFixtureShape(FIXTURE_DESCENDING_STAIRS);
  case LOCATION_DESCENDING_STAIRCASE:
  case LOCATION_ASCENDING_STAIRCASE:
    database->placeRoomItem(ROOM_ITEM_FIXTURE, 6, 6);
    screen.drawDynamicFixture();
    break;
  }
}

void GameEngine::drawLoot()
{
  // G@>ACE5
  if (database->roomHasUnopenedChest(currentRoom)) {
    for(RoomItem n=ROOM_ITEM_MONEY; n<=ROOM_ITEM_LAST; n=next(n))
      if (n != ROOM_ITEM_CENTERPIECE)
	database->copyRoomItemPosition(ROOM_ITEM_CHEST, n);
    screen.drawChestItem();
    return;
  }
  database->clearRoomItemPosition(ROOM_ITEM_CHEST);
  if (database->getRoomMoneyAmount(currentRoom) != 0)
    screen.drawMoneyItem();
  for (unsigned n = 0; n < 3; n++) {
    ItemCategory cat;
    byte id;
    if ((id = database->getRoomLootItem(currentRoom, n, cat)) != 0)
      screen.drawLootItem(n, cat, id);
  }
}

GameEngine::Diversion GameEngine::room(bool newLocation)
{
  // G@>657E
  sound.stopMusic();
  screen.roomScreen();

  if (!newLocation && database->getCurrentLocation() != LOCATION_ENTRANCE) {
    if (database->getCurrentLocation() == LOCATION_ROOM &&
	database->getRoomSpecialType(currentRoom) == 0) {
      drawLoot();
      if (database->isRoomItemPlaced(ROOM_ITEM_CENTERPIECE)) {
	if (database->roomHasLivingStatue(currentRoom))
	  screen.drawLivingStatue();
	if (database->roomHasFountain(currentRoom))
	  screen.drawFountain();
      }
    } else {
      if (database->isRoomItemPlaced(ROOM_ITEM_FIXTURE))
	screen.drawDynamicFixture();
    }
    for (unsigned p=0; p<database->getNumConfiguredPlayers(); p++) {
      if (screen.isPlayerBlocked(p)) {
	byte y;
	byte x;
	pickUnoccupiedRoomSquare(y, x);
	database->placePlayer(p, y, x);
      }
      if (database->getPlayerHP(p) > database->getPlayerWD(p))
	screen.drawPlayer(p);
    }
    if (database->inCombat() || database->getCurrentLocation() == LOCATION_ROOM ||
	database->getCurrentLocation() >= LOCATION_DESCENDING_STAIRCASE) {
      if ((database->getCurrentLocation() == LOCATION_ROOM &&
	   database->getRoomSpecialType(currentRoom) == 0 &&
	   database->roomHasEnemies(currentRoom)) ||
	  database->inCombat()) {
	// FIXME G@>AF7A
      }
    }
  } else {
    if (database->getCurrentLocation() == LOCATION_ENTRANCE)
      lastActionKey = database->getKeymapEntry(KEYMAP_BREAK_DOOR);
    else {
      // FIXME G@>659A
    }
    // G@>659F
    placeRoomItems();
    if (lastActionKey == 'C') // Not using keymap! G@>65A2
      database->setSecretDoorsRevealed(false);
    lastActionKey = 0;
    database->setCurrentPlayer(-1);
  }
  // G@>65B2
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
      return DIVERSION_ENTER_LOCATION;
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
	if (database->getCurrentLocation() != LOCATION_ASCENDING_STAIRCASE)
	  continue;
	database->setCurrentFloor(database->getCurrentFloor()-1);
	if (database->getCurrentFloor() == 0)
	  return DIVERSION_ENTRANCE;
	database->setCurrentLocation(LOCATION_DESCENDING_STAIRCASE);
	screen.stairMovement(true);
	setRoomFixtureShape(FIXTURE_DESCENDING_STAIRS);
	return DIVERSION_STAIRCASE;
      }
      if (keyCode == KEY_DOWN) {
	if (database->getCurrentLocation() != LOCATION_DESCENDING_STAIRCASE &&
	    database->getCurrentLocation() != LOCATION_ENTRANCE)
	  continue;
	/* FIXME: need map to descend */
	database->setCurrentFloor(database->getCurrentFloor()+1);
	database->setCurrentLocation(LOCATION_ASCENDING_STAIRCASE);
	screen.stairMovement(false);
	setRoomFixtureShape(FIXTURE_ASCENDING_STAIRS);
	return DIVERSION_STAIRCASE;
      }
      return DIVERSION_CONTINUE_GAME;
    }
  }
}

GameEngine::Diversion GameEngine::corridor()
{
  MapPosition pos = database->getMapPosition();
  Location loc = database->mapLocation(pos);
  database->setCurrentLocation(loc);
  if (loc == LOCATION_ROOM || loc > LOCATION_FOUNTAIN)
    return DIVERSION_ENTER_LOCATION;
  for (;;) {
    screen.clearMessages();
    /* FIXME: V@>111D */
    pos = database->getMapPosition();
    loc = database->getCurrentLocation();
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
    for (;;) {
      /* G@>675C */

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
	  break;
	}
	if (keyCode == database->getKeymapEntry(KEYMAP_BREAK_DOOR))
	  dir = DIR_NORTH;
      }
      if (dir != DIR_NONE) {
	dir = direction + dir;
	if (dir != direction) {
	  direction = dir;
	  if (tryMove(true))
	    return DIVERSION_CORRIDOR_MAIN;
	}
	if (!tryMove())
	  break;
	loc = database->getCurrentLocation();
	if (loc != LOCATION_ROOM && !(loc >= LOCATION_DESCENDING_STAIRCASE))
	  return DIVERSION_CORRIDOR_MAIN;
	if (keyCode == database->getKeymapEntry(KEYMAP_BREAK_DOOR))
	  lastActionKey = database->getKeymapEntry(KEYMAP_BREAK_DOOR);
	sound.stopMusic();
	for (unsigned n = 0; n < 8; n++) {
	  screen.drawDoorReveal(n);
	  if (n==3) {
	    int s = database->getMonsterSound();
	    if (s >= 0)
	      sound.playMonsterSound(s);
	  }
	  if (lastActionKey != 'B') // Not using keymap! G@>A6B4
	    if (Diversion d = delay(50))
	      return d;
	}
	if (Diversion d = delay(500))
	  return d;
	return DIVERSION_ENTER_LOCATION;
      } else if (keyCode == database->getKeymapEntry(KEYMAP_USE_ITEM)) {
	/* FIXME G@>67DF */
      } else if (keyCode == database->getKeymapEntry(KEYMAP_CHANGE_ORDER)) {
	/* FIXME G@>67E8 */
      } else if (keyCode == database->getKeymapEntry(KEYMAP_LISTEN_AT_DOOR)) {
	/* FIXME G@>67F2 */
      } else if (keyCode == database->getKeymapEntry(KEYMAP_TRADE_ITEMS)) {
	/* FIXME G@>6820 */
      }
    }
  }
}

GameEngine::Diversion GameEngine::core(bool newLocation)
{
  database->setMapVisited(database->getMapPosition(), true);
  if (database->inCombat())
    return room(newLocation);
  else switch(database->getCurrentLocation()) {
    case LOCATION_CORRIDOR:
    case LOCATION_FOUNTAIN:
      return DIVERSION_CORRIDOR;
    case LOCATION_ROOM:
      currentRoom = database->getRoomDescriptor(database->getMapPosition());
    case LOCATION_DESCENDING_STAIRCASE:
    case LOCATION_ASCENDING_STAIRCASE:
    case LOCATION_ENTRANCE:
      return room(newLocation);
    default:
      return DIVERSION_QUIT;
    }
}

}
