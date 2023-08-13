#include "system.h"

#include "Engine.h"
#include "Utils.h"
#include "Database.h"

namespace Tunnels {

GameEngine::Diversion GameEngine::getNamedPlayer()
{
  for(;;) {
    constexpr unsigned nameLen = 15;
    byte name[nameLen];
    if (Diversion d = getString(nameLen, name))
      return d;
    database->setCurrentPlayer(-1);
    if (name[0] == ' ')
      return DIVERSION_NULL;
    while (database->nextPlayerInOrder()) {
      Utils::StringSpan matchName = database->getPlayerName(database->getCurrentPlayer());
      unsigned i;
      for (i = 0; i < matchName.len() && i < nameLen; i++)
	if (name[i] != ' ' && name[i] != matchName[i])
	  break;
      if (i == nameLen) {
	screen.replaceStringInputField(matchName);
	return DIVERSION_NULL;
      }
    }
    sound.honk();
  }
}

void GameEngine::initEnemyHealth()
{
  for (unsigned i = 0; i < database->getNumEnemies(); i++)
    for (unsigned n = database->getMonsterHPNumD6(); n>0; --n) {
      byte roll = random(1, 6);
      byte hp = database->getMonsterHP(i);
      if (hp < 245)
	database->setMonsterHP(i, hp + roll);
    }
}

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
      initEnemyHealth();
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

GameEngine::Diversion GameEngine::partyOrder()
{
  // G@>C1E1
  screen.drawPrompt(0x0e);
  screen.drawPrompt(0x7c);
  for(;;) {
    screen.drawCurrentPartyOrder();
    byte kc;
    if (Diversion d = rawGetKey(kc))
      return d;
    if (kc == KEY_BACK)
      return DIVERSION_CONTINUE_GAME;
    if (kc == 'E')
      database->exchangePlayerOrder(0, 1);
    if (kc == 'S')
      database->exchangePlayerOrder(0, 2);
    if (kc == 'D')
      database->exchangePlayerOrder(1, 3);
    if (kc == 'X')
      database->exchangePlayerOrder(2, 3);
    if (database->getPlayerOrder(0) < 0 && database->getPlayerOrder(1) < 0) {
      database->exchangePlayerOrder(0, 2);
      database->exchangePlayerOrder(1, 3);
    }
  }
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
	screen.drawPrompt(0x6b, player);
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
	screen.drawPrompt(0x6c, player);
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
  roomDone = 0;
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
      return;
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
    if (database->getNumEnemies() > 0)
      for (unsigned n=0; n<7; n++)
	if (database->getMonsterHP(n) != 0) {
	  byte y, x;
	  pickUnoccupiedRoomSquare(y, x);
	  database->placeMonster(n, y, x);
	  screen.drawMonster(n);
	}
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
    byte id, stat, ammo;
    if ((id = database->getRoomLootItem(currentRoom, n, cat, stat, ammo)) != 0)
      screen.drawLootItem(n, cat, id);
  }
}

bool GameEngine::takeArmor(unsigned player, byte item, bool &inventoryFull)
{
  if (!item)
    return true;
  // FIXME: G@>A458
  if (database->getPlayerArmorProtection(player) != 0) {
    inventoryFull = true;
    return false;
  }
  database->setPlayerArmor(player, item);
  return true;
}

bool GameEngine::takeShield(unsigned player, byte item, bool &inventoryFull)
{
  if (!item)
    return true;
  // FIXME: G@>A472
  if (database->getPlayerShieldProtection(player) != 0) {
    inventoryFull = true;
    return false;
  }
  database->setPlayerShield(player, item);
  return true;
}

bool GameEngine::takeWeapon(unsigned player, ItemCategory cat, byte item,
			    bool &secondary, bool &inventoryFull)
{
  if (!item)
    return true;
  // FIXME: G@>A4AF
  ItemCategory oldCat;
  secondary = (database->getPlayerWeapon(player, false, oldCat) != 0);
  if (database->getPlayerWeapon(player, secondary, oldCat) != 0) {
    inventoryFull = true;
    return false;
  }
  database->setPlayerWeapon(player, secondary, cat, item);
  return true;
}

bool GameEngine::takeMagicItem(unsigned player, byte item, unsigned &slot)
{
  // G@>A507
  if (!item)
    return true;
  byte remUses = database->getMagicItemInitialUses(item);
  if (!remUses) {
    // FIXME: G@>A528
  }
  for (slot=0; slot<10; slot++)
    if (!database->getPlayerMagicItemId(player, slot)) {
      database->setPlayerMagicItemId(player, slot, item);
      database->setPlayerMagicItemRemainingUses(player, slot, remUses);
      return true;
    }
  return false;
}


GameEngine::Diversion GameEngine::startTrade()
{
  screen.drawTradingScreen(0);
  do {
    screen.drawPrompt(0x49);
    acceptMask = ACCEPT_ALPHANUMERIC | ACCEPT_NUMERIC | ACCEPT_BACK;
    if (Diversion d = getNamedPlayer())
      return d;
  } while (database->getCurrentPlayer() < 0);
  screen.prepareTradingNumberInput();
  unsigned catNum;
  if (Diversion d = getNumber(0, 4, catNum))
    return d;
  ItemCategory cat;
  switch (catNum) {
  case 1: cat = ITEM_ARMORS; break;
  case 2: cat = ITEM_SHIELDS; break;
  case 3: cat = ITEM_WEAPONS; break;
  case 4: cat = ITEM_MAGIC_ITEMS; break;
  default:
    return DIVERSION_CONTINUE_GAME;
  }
  currentItemCategory = cat;
  currentItem = 0;
  currentItemStat = 0;
  currentItemAmmo = 0;
  return DIVERSION_TRADE_ITEM;
}

GameEngine::Diversion GameEngine::tradeItem(ItemCategory cat, byte item,
					    byte itemStat, byte itemAmmo)
{
  // G@>6F52
  struct { ItemCategory cat; byte id; byte stat; byte ammo; } items[11];
  items[0].cat = cat;
  items[0].id = item;
  items[0].stat = itemStat;
  items[0].ammo = itemAmmo;
  acceptMask = ACCEPT_NUMERIC;
  int currentPlayer = database->getCurrentPlayer();
  unsigned numItems = 1;
  if (item)
    numItems = 0;
  else
    acceptMask |= ACCEPT_BACK;
  screen.drawTradingScreen(item);
  switch (cat) {
  case ITEM_ARMORS:
    if (item)
      screen.drawArmorChoice(item, itemStat, false);
    screen.draw1Choice();
    items[1].cat = ITEM_ARMORS;
    items[1].id = database->getPlayerArmorId(currentPlayer);
    items[1].stat = database->getPlayerArmorProtection(currentPlayer);
    items[1].ammo = 0;
    screen.drawArmorChoice(items[1].id, items[1].stat, false);
    numItems = 1;
    break;
  case ITEM_SHIELDS:
    if (item)
      screen.drawArmorChoice(item, itemStat, true);
    screen.draw1Choice();
    items[1].cat = ITEM_SHIELDS;
    items[1].id = database->getPlayerShieldId(currentPlayer);
    items[1].stat = database->getPlayerShieldProtection(currentPlayer);
    items[1].ammo = 0;
    screen.drawArmorChoice(items[1].id, items[1].stat, true);
    numItems = 1;
    break;
  case ITEM_WEAPONS:
  case ITEM_RANGED_WEAPONS:
    if (item)
      screen.drawWeaponChoice(cat, item, itemStat, itemAmmo);
    screen.draw2Choice();
    for (unsigned i=0; i<2; i++) {
      items[i+1].id = database->getPlayerWeapon(currentPlayer, i,
						items[i+1].cat);
      items[i+1].stat = database->getPlayerWeaponDamage(currentPlayer, i);
      items[i+1].ammo = database->getPlayerWeaponAmmo(currentPlayer, i);
      if (!items[i+1].stat)
	items[i+1].stat = 2;
      screen.drawWeaponChoice(items[i+1].cat, items[i+1].id,
			      items[i+1].stat, items[i+1].ammo);
    }
    numItems = 2;
    break;
  case ITEM_MAGIC_ITEMS:
    if (item)
      screen.drawMagicItemChoice(item, itemStat);
    screen.drawPrompt(0x46);
    for (unsigned i=0; i<10; i++) {
      items[i+1].cat = ITEM_MAGIC_ITEMS;
      items[i+1].id = database->getPlayerMagicItemId(currentPlayer, i);
      items[i+1].stat = database->getPlayerMagicItemRemainingUses(currentPlayer, i);
      items[i+1].ammo = 0;
      screen.drawMagicItemChoice(items[i+1].id, items[i+1].stat);
    }
    numItems = 10;
    break;
  }
  unsigned choice = 0;
  if (numItems == 1 && !item) {
    screen.drawPrompt(0x82);
    if (items[1].id)
      choice = 1;
    else
      sound.honk();
  }
  if (!choice)
    do {
      screen.prepareTradingNumberInput();
      if (Diversion d = getNumber(0, numItems, choice))
	return d;
      if (!items[choice].id)
	sound.honk();
    } while (!items[choice].id);
  currentItemCategory = items[choice].cat;
  currentItem = items[choice].id;
  currentItemStat = items[choice].stat;
  currentItemAmmo = items[choice].ammo;
  if (choice != 0)
    items[choice] = items[0];
  switch (cat) {
  case ITEM_ARMORS:
    database->setPlayerArmor(currentPlayer, items[1].id);
    database->setPlayerArmorProtection(currentPlayer, items[1].stat);
    break;
  case ITEM_SHIELDS:
    database->setPlayerShield(currentPlayer, items[1].id);
    database->setPlayerShieldProtection(currentPlayer, items[1].stat);
    break;
  case ITEM_WEAPONS:
  case ITEM_RANGED_WEAPONS:
    database->setPlayerWeapon(currentPlayer, false, items[1].cat, items[1].id);
    database->setPlayerWeaponDamage(currentPlayer, false, items[1].stat);
    database->setPlayerWeaponAmmo(currentPlayer, false, items[1].ammo);
    database->setPlayerWeapon(currentPlayer, true, items[2].cat, items[2].id);
    database->setPlayerWeaponDamage(currentPlayer, true, items[2].stat);
    database->setPlayerWeaponAmmo(currentPlayer, true, items[2].ammo);
    break;
  case ITEM_MAGIC_ITEMS:
    for (unsigned i=0; i<10; i++) {
      database->setPlayerMagicItemId(currentPlayer, i, items[i+1].id);
      database->setPlayerMagicItemRemainingUses(currentPlayer, i, items[i+1].stat);
    }
    break;
  }
  return DIVERSION_GIVE_ITEM;
}

GameEngine::Diversion GameEngine::giveItem(ItemCategory cat, byte item,
					   byte itemStat, byte itemAmmo)
{
  // G@>7179
  if (!item)
    return backTarget;
  acceptMask = ACCEPT_NUMERIC | ACCEPT_ALPHANUMERIC;
  int currentPlayer = database->getCurrentPlayer();
  screen.drawPrompt(0x4a);
  screen.prepareGiveItemReceiverInput();
  if (Diversion d = getNamedPlayer())
    return d;
  int player = database->getCurrentPlayer();
  if (player >= 0) {
    bool inventoryFull = false;
    switch (cat) {
    case ITEM_ARMORS:
      if (takeArmor(player, item, inventoryFull)) {
	if (itemStat != 0)
	  database->setPlayerArmorProtection(player, itemStat);
	return backTarget;
      }
      break;
    case ITEM_SHIELDS:
      if (takeShield(player, item, inventoryFull)) {
	if (itemStat != 0)
	  database->setPlayerShieldProtection(player, itemStat);
	return backTarget;
      }
      break;
    case ITEM_WEAPONS:
    case ITEM_RANGED_WEAPONS:
      bool secondary;
      if (takeWeapon(player, cat, item, secondary, inventoryFull)) {
	if (itemStat != 0)
	  database->setPlayerWeaponDamage(player, secondary, itemStat);
	return backTarget;
      }
      break;
    case ITEM_MAGIC_ITEMS:
      unsigned slot;
      if (takeMagicItem(player, item, slot)) {
	if (itemStat != 0)
	  database->setPlayerMagicItemRemainingUses(player, slot, itemStat);
	return backTarget;
      }
      inventoryFull = true;
      break;
    }
    if (inventoryFull) {
      if (itemStat != 0) {
	// FIXME: G@>72A3
      }
      currentItemCategory = cat;
      currentItem = item;
      currentItemStat = itemStat;
      currentItemAmmo = itemAmmo;
      return DIVERSION_TRADE_ITEM;
    }
    database->setCurrentPlayer(currentPlayer);
    if (database->getNumConfiguredPlayers() > 1) {
      sound.honk();
      return DIVERSION_GIVE_ITEM;
    }
  }

  if (database->getCurrentLocation() != LOCATION_ROOM ||
      database->getRoomSpecialType(currentRoom) != 0 ||
      database->roomHasUnopenedChest(currentRoom) ||
      !database->dropItemInRoom(currentRoom, cat, item, itemStat, itemAmmo)) {
    screen.drawPrompt(0x4b);
    database->setCurrentPlayer(currentPlayer);
    procdTarget = backTarget;
    redoTarget = DIVERSION_GIVE_ITEM;
    acceptMask = ACCEPT_PROCD | ACCEPT_REDO;
    byte kc;
    return getKeyNoCursor(kc);
  }

  return backTarget;
}

void GameEngine::populateStoreList(byte (&ids)[10], uint16 (&prices)[10], unsigned &index, unsigned &selectable, ItemCategory cat, byte id)
{
  int player = database->getCurrentPlayer();
  uint16 price = database->getItemStorePrice(cat, id);
  if (price) {
    bool isSelectable =
      (player >= 0 && database->playerCanUseItem(player, cat, id));
    screen.drawStoreItem(cat, id, isSelectable);
    ids[index] = id;
    prices[index] = price;
    ids[++index] = 0;
    if (isSelectable)
      selectable |= 1U << index;
  } else {
    if (cat != ITEM_RANGED_WEAPONS ||
	!database->getRangedWeaponAmmoStorePrice(id))
      return;
    screen.drawStoreItem(cat, 0);
    ids[index] = id;
    prices[index] = 0;
    ids[++index] = 0;
  }
  if (cat == ITEM_RANGED_WEAPONS) {
    bool isSelectable = false;
    price = database->getRangedWeaponAmmoStorePrice(id);
    ids[index] = id;
    if (!price || !(database->getRangedWeaponAmmoType(id) & 1))
      id = 0;
    else if (player >= 0) {
      ItemCategory cat2;
      if ((database->getPlayerWeapon(player, false, cat2) == id && cat2 == cat) ||
	  (database->getPlayerWeapon(player, true, cat2) == id && cat2 == cat))
	isSelectable = true;
    }
    screen.drawStoreItem(ITEM_RANGED_WEAPONS, id, isSelectable, true);
    prices[index] = price;
    ids[++index] = 0;
    if (isSelectable)
      selectable |= 1U << index;
  }
}

GameEngine::Diversion GameEngine::generalStore()
{
  // G@>6CC8
  redoTarget = DIVERSION_GENERAL_STORE;
  backTarget = DIVERSION_GENERAL_STORE;
  unsigned price, choice, category = 0;
  bool keepState = false;
  for (;;) {
    if (keepState)
      keepState = false;
    else {
      if (category > 5)
	return DIVERSION_GENERAL_STORE;
      roomDone = -1;
      if (database->getPartyGold() == 0)
	return DIVERSION_ROOM_SETUP;
      screen.drawPrompt(0x3d);
      database->setCurrentPlayer(-1);
      choice = 0;
      price = 0;
    }
    byte listIds[10];
    uint16 listPrices[10];
    unsigned listIndex = 0, selectable = 0;
    screen.prepareStoreItemList();
    switch (category) {
    case 0:
      screen.drawPrompt(0x3e);
      procdTarget = DIVERSION_ROOM_SETUP;
      acceptMask = ACCEPT_PROCD | ACCEPT_REDO | ACCEPT_NUMERIC;
      if (Diversion d = getNumber(1, 7, category))
	return d;
      if (category < 6)
	continue;
      if (category == 6) {
	// Rations
	price = database->getRationPrice();
	if (!price)
	  continue;
	if (database->getRations() >= 100) {
	  screen.drawPrompt(0x44);
	  break;
	}
	if (database->getPartyGold() < price) {
	  screen.drawPrompt(0x42);
	  break;
	}
	database->setRations(database->getRations() + database->getRationQuantum());
      } else {
	// Healing
	price = database->getHealingPrice();
	if (database->getPartyGold() < price) {
	  screen.drawPrompt(0x42);
	  break;
	}
	screen.drawPrompt(0x3b);
	acceptMask = ACCEPT_PROCD | ACCEPT_REDO | ACCEPT_ALPHANUMERIC;
	if (Diversion d = getNamedPlayer())
	  return d;
	int player = database->getCurrentPlayer();
	if (player < 0 || database->getPlayerWD(player) == 0)
	  return DIVERSION_GENERAL_STORE;
	database->setPlayerWD(player, database->getPlayerWD(player) >> 1);
      }
      database->setPartyGold(database->getPartyGold() - price);
      screen.drawPrompt(0x45);
      break;
    case 1:
      // Weapons
      for (byte id = 1; id <= 8; id++)
	populateStoreList(listIds, listPrices, listIndex, selectable,
			  ITEM_WEAPONS, id);
      break;
    case 2:
    case 3:
      // Ranged weapons
      {
	byte id = (category == 3? 5 : 1);
	do
	  populateStoreList(listIds, listPrices, listIndex, selectable,
			    ITEM_RANGED_WEAPONS, id);
	while((++id & 3) != 1);
      }
      break;
    case 4:
      // Armors
      for (byte id = 1; id <= 8; id++)
	populateStoreList(listIds, listPrices, listIndex, selectable,
			  ITEM_ARMORS, id);
      break;
    case 5:
      // Shields
      for (byte id = 1; id <= 6; id++)
	populateStoreList(listIds, listPrices, listIndex, selectable,
			  ITEM_SHIELDS, id);
      break;
    }
    if (category < 6) {
      for (;;) {
	int player;
	if (!listIndex) {
	  category = 0;
	  screen.drawPrompt(0x3c);
	} else if ((player = database->getCurrentPlayer()) >= 0) {
	  if (choice) {
	    bool inventoryFull = false, success = false, secondary;
	    currentItem = listIds[choice-1];
	    switch (category) {
	    case 2:
	    case 3:
	      if (!(choice & 1)) {
		ItemCategory cat;
		if (currentItem ==
		    database->getPlayerWeapon(player, false, cat) &&
		    cat == ITEM_RANGED_WEAPONS)
		  secondary = false;
		else if (currentItem ==
			 database->getPlayerWeapon(player, true, cat) &&
			 cat == ITEM_RANGED_WEAPONS)
		  secondary = true;
		else
		  break;
		success = true;
		if (database->getPlayerWeaponAmmo(player, secondary) > 100)
		  inventoryFull = true;
		else
		  database->setPlayerWeaponAmmo(player, secondary,
						database->getPlayerWeaponAmmo(player, secondary) +
						database->getAmmoQuantum());
		break;
	      }
	    case 1:
	      currentItemCategory = (category == 1? ITEM_WEAPONS :
				     ITEM_RANGED_WEAPONS);
	      success = takeWeapon(player, currentItemCategory,
				   currentItem, secondary, inventoryFull);
	      break;
	    case 4:
	      currentItemCategory = ITEM_ARMORS;
	      success = takeArmor(player, currentItem, inventoryFull);
	      break;
	    case 5:
	      currentItemCategory = ITEM_SHIELDS;
	      success = takeShield(player, currentItem, inventoryFull);
	      break;
	    }
	    if (success) {
	      if (inventoryFull) {
		screen.drawPrompt(0x44);
		database->setPartyGold(database->getPartyGold() + price);
	      } else
		screen.drawPrompt(0x45);
	    } else if (!inventoryFull) {
	      screen.drawPrompt(0x43);
	      choice = 0;
	    } else {
	      database->getDefaultItemStats(currentItemCategory, currentItem,
					    currentItemStat, currentItemAmmo);
	      return DIVERSION_TRADE_ITEM;
	    }
	  } else {
	    if (listIndex == 1)
	      choice = 1;
	    else {
	      screen.drawPrompt(0x3f);
	      Diversion d = getNumber(0, listIndex, choice);
	      if (d == DIVERSION_REDO) {
		choice = 0;
		continue;
	      }
	      if (d)
		return d;
	    }
	    if (!(selectable & (1U << choice))) {
	      screen.drawPrompt(0x43);
	      choice = 0;
	    } else {
	      price = listPrices[choice-1];
	      if (database->getPartyGold() < price)
		screen.drawPrompt(0x42);
	      else {
		database->setPartyGold(database->getPartyGold() - price);
		keepState = true;
	      }
	    }
	  }
	} else {
	  screen.drawPrompt(0x3b);
	  redoTarget = DIVERSION_REDO;
	  acceptMask = ACCEPT_BACK | ACCEPT_REDO | ACCEPT_ALPHANUMERIC;
	  Diversion d = getNamedPlayer();
	  if (d == DIVERSION_REDO) {
	    choice = 0;
	    continue;
	  }
	  if (d)
	    return d;
	  int player = database->getCurrentPlayer();
	  if (player < 0)
	    return DIVERSION_GENERAL_STORE;
	  keepState = true;
	}
	break;
      }
      if (keepState)
	continue;
    }
    if (Diversion d = flashBorder())
      return d;
  }
}

GameEngine::Diversion GameEngine::lootFixtures()
{
  if /* while */ (database->roomHasFountain(currentRoom)) {
    // FIXME: G@>C68B
  }
  if /* while */ (database->roomHasLivingStatue(currentRoom)) {
    // FIXME: G@>C699
  }
  roomDone = -1;
  return DIVERSION_ROOM_MAIN;
}

GameEngine::Diversion GameEngine::lootItems()
{
  redoTarget = DIVERSION_LOOT_FIRST_ITEM;
  procdTarget = DIVERSION_LOOT_FIXTURES;
  backTarget = DIVERSION_LOOT_NEXT_ITEM;
  screen.clearMessages();
  int slot;
  if ((slot = database->getRoomNextLootSlot(currentRoom, itemIterPos)) < 0)
    return DIVERSION_LOOT_FIXTURES;
  ItemCategory cat;
  byte itemStat, itemAmmo;
  byte item = database->getRoomLootItem(currentRoom, slot, cat, itemStat, itemAmmo);
  switch(cat) {
  case ITEM_MAGIC_ITEMS:
  case ITEM_ARMORS:
  case ITEM_SHIELDS:
  case ITEM_WEAPONS:
  case ITEM_RANGED_WEAPONS:
    screen.drawLootItemName(cat, item);
    break;
  case ITEM_FLOOR_MAP:
    screen.drawPrompt(0x5b);
    if (database->getCurrentFloor() > database->getMappedFloors())
      database->setMappedFloors(database->getCurrentFloor());
    break;
  case ITEM_QUEST_OBJECTS:
    if (database->tryAchieveQuestObject(item)) {
      sound.playQuestObjectCompleteMusic();
      screen.drawPrompt(0x5c, item);
    } else {
      screen.drawPrompt(0x19, item);
      sound.playQuestObjectFailedMusic();
    }
    break;
  }
  for (unsigned i = 0; i < 3; i++) {
    screen.clearLootItem(slot);
    if (Diversion diversion = delay(133))
      return diversion;
    screen.drawLootItem(slot, cat, item);
    if (Diversion diversion = delay(133))
      return diversion;
  }
  database->clearRoomLootSlot(currentRoom, itemIterPos, slot);
  if (cat >= ITEM_QUEST_OBJECTS)
    return DIVERSION_LOOT_ITEM_DONE;
  else {
    backTarget = DIVERSION_LOOT_ITEM_DONE;
    currentItemCategory = cat;
    currentItem = item;
    currentItemStat = itemStat;
    currentItemAmmo = itemAmmo;
    return DIVERSION_GIVE_ITEM;
  }
}

GameEngine::Diversion GameEngine::lootRoom()
{
  // G@>C41F
  if (database->getCurrentLocation() == LOCATION_ROOM &&
      database->getRoomSpecialType(currentRoom) == 0) {
    screen.clearMessages();
    while (database->roomHasUnopenedChest(currentRoom)) {
      screen.drawPrompt(0x58);
      acceptMask = ACCEPT_ALPHANUMERIC;
      if (Diversion d = getNamedPlayer())
	return d;
      int player = database->getCurrentPlayer();
      if (player < 0) {
	roomDone = -1;
	return DIVERSION_ROOM_MAIN;
      }
      if (database->getPlayerWD(player) >= database->getPlayerHP(player)) {
	screen.clearMessages();
	continue;
      }
      if (database->roomHasTrap(currentRoom)) {
	// FIXME: G@>C455
      }
      database->clearRoomChestAndTrap(currentRoom);
      database->clearRoomItemPosition(ROOM_ITEM_CHEST);
      break;
    }
    if (byte gold = database->getRoomMoneyAmount(currentRoom)) {
      database->setPartyGold(database->getPartyGold() + gold);
      screen.drawMoneyItem();
      screen.clearMessages();
      screen.drawPrompt(0x59, gold);
      database->setRoomMoneyAmount(currentRoom, 0);
      database->clearRoomItemPosition(ROOM_ITEM_MONEY);
      if (Diversion d = flashBorder())
	return d;
    }
    return DIVERSION_LOOT_FIRST_ITEM;
  }
  // G@>C424
  roomDone = -1;
  return DIVERSION_ROOM_MAIN;
}

void GameEngine::roomSetup(bool newLocation)
{
  // G@>657E
  sound.stopMusic();
  screen.roomScreen();
  if (database->getCurrentLocation() == LOCATION_ROOM) {
    database->setRoomVisited(currentRoom);
    if (database->getRoomSpecialType(currentRoom) == 2)
      screen.drawVault();
  }

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
	for (unsigned n=0; n<8; n++)
	  if (database->getMonsterHP(n) != 0) {
	    if (!database->isMonsterPlaced(n))
	      database->setMonsterHP(n, 0);
	    else {
	      if (screen.isMonsterBlocked(n)) {
		byte y;
		byte x;
		pickUnoccupiedRoomSquare(y, x);
		database->placeMonster(n, y, x);
	      }
	      screen.drawMonster(n);
	    }
	  }
      }
    }
  } else {
    if (database->getCurrentLocation() == LOCATION_ENTRANCE)
      lastActionKey = database->getKeymapEntry(KEYMAP_BREAK_DOOR);
    else
      roomDone = database->getUnknown10FB();
    // G@>659F
    placeRoomItems();
    if (lastActionKey == 'C') // Not using keymap! G@>65A2
      database->setSecretDoorsRevealed(false);
    lastActionKey = 0;
    database->setCurrentPlayer(-1);
  }
}

GameEngine::Diversion GameEngine::room()
{
  // G@>65B2
  if (roomDone != -1 && database->getCurrentLocation() == LOCATION_ROOM) {
    switch (database->getRoomSpecialType(currentRoom)) {
    case 2:
      // Vault
      return DIVERSION_VAULT;
    case 1:
      // General store
      if (database->getPartyGold() != 0 &&
	  roomDone >= 0) {
	sound.playGeneralStoreMusic();
	if (Diversion d = waitForMusic())
	  return d;
	return DIVERSION_GENERAL_STORE;
      }
      break;
    case 0:
      // Normal room
      if (database->roomHasEnemies(currentRoom))
	database->startCombat(currentRoom);
      else {
	database->clearCombat();
	return DIVERSION_LOOT_ROOM;
      }
      break;
    }
  }
  if (database->getNumEnemies() > 0)
    ; // FIXME: Go to G@>682E
  if (database->getCurrentLocation() == LOCATION_ENTRANCE) {
    if (!database->isAnyQuestObjectRemaining()) {
      backTarget = DIVERSION_CONTINUE_GAME;
      database->clearRemainingQuestObjects();
      database->setUnknown1CEB(0);
      screen.drawGameOver();
      database->revealAllMagicItems();
      sound.playTitleMusic();
      byte keyCode;
      Direction dir;
      for (;;)
	if (Diversion d = getMovementKey(keyCode, dir))
	  return d;
    }
    if (!(database->getUnknown25EF() & 0x80) &&
	database->getPartyGold() != 0 &&
	roomDone >= 0) {
      sound.playGeneralStoreMusic();
      if (Diversion d = waitForMusic())
	return d;
      return DIVERSION_GENERAL_STORE;
    }
  }
  // G@>663F
  for (;;) {
    backTarget = DIVERSION_CONTINUE_GAME;
    // FIXME: Clear V@>111D, current player character sheet addr, @>833C
    database->clearCombat();
    // FIXME: Clear @>8330, selected item number, selected magical item id
    // FIXME: Clear character sheet address (VDP), V@>3248
    lastActionKey = 0;
    screen.clearMessages();
    screen.drawPrompt(0x2e);
    byte keyCode;
    Direction dir;
    if (Diversion d = getMovementKey(keyCode, dir))
      return d;
    if (dir != DIR_NONE) {
      direction = dir;
      if (database->getCurrentLocation() == LOCATION_ENTRANCE)
	continue;
      roomDone = 0;
      if (!tryMove())
	continue;
      return DIVERSION_ENTER_LOCATION;
    } else {
      if (keyCode == database->getKeymapEntry(KEYMAP_USE_ITEM)) {
	roomDone = 0;
	// FIXME: Go to G@>771C
      }
      if (keyCode == database->getKeymapEntry(KEYMAP_CHANGE_ORDER)) {
	return DIVERSION_PARTY_ORDER;
      }
      if (keyCode == database->getKeymapEntry(KEYMAP_TRADE_ITEMS)) {
	return startTrade();
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
	if (database->hasHiddenMap() &&
	    !(database->getMappedFloors() >= database->getCurrentFloor())) {
	  screen.drawPrompt(0x2d);
	  if (Diversion d = flashBorder())
	    return d;
	  continue;
	}
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
  roomDone = 0;
  for (;;) {
    screen.clearMessages();
    /* FIXME: Clear V@>111D */
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
	return DIVERSION_PARTY_ORDER;
      } else if (keyCode == database->getKeymapEntry(KEYMAP_LISTEN_AT_DOOR)) {
	if (random(1, 100) <= database->getListenAtDoorSuccessRate() &&
	    tryMove(true) && currentRoom != invalidHandle() &&
	    database->roomHasEnemies(currentRoom)) {
	  int s = database->getMonsterSound();
	  if (s >= 0)
	    sound.playMonsterSound(s);
	}
	database->clearCombat();
	/* FIXME G@>6813 */
	return DIVERSION_CORRIDOR_MAIN;
      } else if (keyCode == database->getKeymapEntry(KEYMAP_TRADE_ITEMS)) {
	backTarget = DIVERSION_CORRIDOR;
	return startTrade();
      }
    }
  }
}

GameEngine::Diversion GameEngine::core(bool newLocation)
{
  database->setMapVisited(database->getMapPosition(), true);
  if (!database->inCombat())
    switch(database->getCurrentLocation()) {
    case LOCATION_CORRIDOR:
    case LOCATION_FOUNTAIN:
      return DIVERSION_CORRIDOR;
    case LOCATION_ROOM:
      currentRoom = database->getRoomDescriptor(database->getMapPosition());
    case LOCATION_DESCENDING_STAIRCASE:
    case LOCATION_ASCENDING_STAIRCASE:
    case LOCATION_ENTRANCE:
      break;
    default:
      return DIVERSION_QUIT;
    }
  roomSetup(newLocation);
  return room();
}

}
