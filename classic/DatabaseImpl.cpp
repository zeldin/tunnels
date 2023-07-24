#include "system.h"

#include "classic/DatabaseImpl.h"
#include "RandomSource.h"
#include "Utils.h"

namespace Tunnels { namespace Classic {

Utils::StringSpan DatabaseImpl::getSpritePatternTable() const
{
  return data.spritePatternTable;
}

Utils::StringSpan DatabaseImpl::getDescription() const
{
  return data.description;
}

Utils::StringSpan DatabaseImpl::getPatternTable() const
{
  return data.patternTable;
}

Utils::StringSpan DatabaseImpl::getHighPatternTable(bool alternate) const
{
  if (alternateHighPatternsActive())
    alternate = !alternate;
  if (alternate)
    return data.patternTable2;
  Utils::StringSpan patterns{data.patternTable};
  patterns.subspan(0x400, 0x380);
  return patterns;
}

void DatabaseImpl::clearRoomFixtures()
{
  Utils::clearArray(data.patternTable, 240*8, 8*8);
}

void DatabaseImpl::setRoomFixture(RoomFixture fixture)
{
  Utils::StringSpan patterns = getHighPatternTable(false);
  patterns.subspan(unsigned(fixture)*(4*8), 4*8);
  patterns.store(data.patternTable, 244*8);
}

void DatabaseImpl::clearPlayerSheet(unsigned n)
{
  /* FIXME */
  data.player[n].flags = 0;
}

Utils::StringSpan DatabaseImpl::getPlayerName(unsigned n) const
{
  return data.player[n].name;
}

void DatabaseImpl::setPlayerName(unsigned n, Utils::StringSpan name)
{
  name.store(data.player[n].name);
}

void DatabaseImpl::setPlayerArmor(unsigned n, byte item)
{
  if (!item || item > 8)
    return;
  data.player[n].armorId = item;
  data.player[n].armorProtection = data.armors[item-1].protection;
}

void DatabaseImpl::setPlayerShield(unsigned n, byte item)
{
  if (item > 8)
    item -= 8;
  if (!item || item > 6)
    return;
  data.player[n].shieldId = item + 8;
  data.player[n].shieldProtection = data.shields[item-1].protection;
}

byte DatabaseImpl::getPlayerWeapon(unsigned n, bool secondary, ItemCategory &cat) const
{
  byte id = (secondary? data.player[n].secondaryWeaponId :
	     data.player[n].primaryWeaponId);
  if (id > 8) {
    id -= 8;
    cat = ITEM_RANGED_WEAPONS;
  } else
    cat = ITEM_WEAPONS;
  return id;
}

void DatabaseImpl::setPlayerWeapon(unsigned n, bool secondary, ItemCategory cat, byte item)
{
  if (!item || (cat != ITEM_WEAPONS && cat != ITEM_RANGED_WEAPONS))
    return;
  byte damage, ammo;
  if (item > 8) {
    item -= 8;
    if (item > 8)
      return;
    cat = ITEM_RANGED_WEAPONS;
  }
  if (cat == ITEM_RANGED_WEAPONS) {
    damage = data.rangedWeapons[item-1].damage;
    ammo = (data.rangedWeapons[item-1].defaultAmmo < 0?
	    -data.rangedWeapons[item-1].defaultAmmo :
	    data.rangedWeapons[item-1].defaultAmmo);
    if (!(ammo >= data.unknown_1ce5))
      ammo = data.unknown_1ce5;
    item += 8;
  } else {
    damage = data.weapons[item-1].damage;
    ammo = 0;
  }
  if (!secondary) {
    data.player[n].primaryWeaponId = item;
    data.player[n].primaryWeaponDamage = damage;
    data.player[n].primaryWeaponAmmo = ammo;
  } else {
    data.player[n].secondaryWeaponId = item;
    data.player[n].secondaryWeaponDamage = damage;
    data.player[n].secondaryWeaponAmmo = ammo;
  }
}

void DatabaseImpl::setPlayerClass(unsigned n, unsigned c)
{
  data.player[n].flags = c << 6;
  for (unsigned i = 0; i < 64; i++)
    data.patternTable[(n << 6) | i] = data.classPatternTable[c][i];
}

void DatabaseImpl::placePlayer(unsigned n, unsigned y, unsigned x)
{
  data.player[n].row = 2*y+1;
  data.player[n].column = 2*x+3;
}

void DatabaseImpl::setPlayerStartPosition(unsigned n, StartPosition pos, Direction dir)
{
  static const byte partyPositions[3][4][4][2] =
    {{{{11, 9}, {11,11}, {13, 9}, {13,11}},
      {{ 7, 7}, { 9, 7}, { 7, 5}, { 9, 5}},
      {{ 5,11}, { 5, 9}, { 3,11}, { 3, 9}},
      {{ 9,13}, { 7,13}, { 9,15}, { 7,15}}},
     {{{11, 9}, {11,11}, {11, 7}, {11,13}},
      {{ 7, 7}, { 9, 7}, { 5, 7}, {11, 7}},
      {{ 5,11}, { 5, 9}, { 5,13}, { 5, 7}},
      {{ 9,13}, { 7,13}, {11,13}, { 5,13}}},
     {{{13, 9}, {13,11}, {16, 9}, {16,11}},
      {{ 7, 5}, { 9, 5}, { 7, 2}, { 9, 2}},
      {{ 3,11}, { 3, 9}, { 0,11}, { 0, 9}},
      {{ 9,15}, { 7,15}, { 9,18}, { 7,18}}}};
  for (unsigned order=0; order<4; order++)
    if (data.playerOrder[order] == n+1) {
      data.player[n].row = partyPositions[pos][dir][order][0];
      data.player[n].column = partyPositions[pos][dir][order][1];
      return;
    }
}

void DatabaseImpl::compactPlayerMagicItems(unsigned n)
{
  unsigned i, j=0;
  for (i=0; i<8; i++)
    if (data.player[n].magicItems[i].id) {
      if (j < i)
	data.player[n].magicItems[j] = data.player[n].magicItems[i];
      j++;
    }
  while (j<8) {
    data.player[n].magicItems[j].id = 0;
    data.player[n].magicItems[j].remainingUses = 0;
    j++;
  }
}

void DatabaseImpl::revealAllMagicItems()
{
  for (unsigned n=0; n<4; n++)
    for (unsigned i=0; i<10; i++)
      if ((data.player[n].magicItems[i].id & 0x80))
	data.player[n].magicItems[i].id = -data.player[n].magicItems[i].id;
}

bool DatabaseImpl::tryAchieveQuestObject(unsigned n)
{
  if (n--) {
    byte mask = byte(1) << n;
    if ((data.remainingQuestObjects & mask)) {
      data.foundQuestObjects |= mask;
      data.remainingQuestObjects ^= mask;
      return true;
    }
  }
  return false;
}

Utils::StringSpan DatabaseImpl::getMonsterName() const
{
  return data.monsterName;
}

int DatabaseImpl::getMonsterSound()
{
  if (data.monsterInfo < 0x20)
    return -1;
  byte snd = data.monsterSoundTable[data.monsterSound];
  if (snd > 16)
    return -1;
  static const uint16 addrTable[] = {
    0xc745,  0xc75b,  0xc771,  0xc787,  0xc79d,  0xc7b3, 0xc7c8,  0xc7de,
    0xc7f4,  0xc809,  0xc81f,  0xc835,  0xc84d,  0xc864, 0xc87a,  0xc890,
    0xc9fc,
  };
  data.monsterSoundAddress = addrTable[snd];
  return snd;
}

void DatabaseImpl::placeMonster(unsigned n, unsigned y, unsigned x)
{
  data.monsterPosition[n].row = 2*y+1;
  data.monsterPosition[n].column = 2*x+3;
}

Utils::StringSpan DatabaseImpl::getClassName(unsigned n) const
{
  return data.classes[n].name;
}

Utils::StringSpan DatabaseImpl::getClassPatternTable(unsigned n) const
{
  return data.classPatternTable[n];
}

Utils::StringSpan DatabaseImpl::getSpecialAttackName(unsigned n) const
{
  if (n>0 && n<=sizeof(data.specialAttacks)/sizeof(data.specialAttacks[0]))
    return data.specialAttacks[n-1].name;
  else
    return Utils::StringSpan();
}

byte DatabaseImpl::getNumClassChoices() const
{
  byte n = data.numClasses;
  if ((n & 0x80)) {
    n = -n;
    if (data.numPlayers != 1)
      --n;
  }
  return n;
}

bool DatabaseImpl::nextPlayerInOrder()
{
  unsigned order = 0;
  if (data.currentPlayer > data.numConfPlayers)
    data.currentPlayer = 0;
  if (data.currentPlayer > 0) {
    while(order < 4 && data.playerOrder[order] != data.currentPlayer)
      order++;
    order++;
  }
  while(order < 4 && !data.playerOrder[order])
    order++;
  if (order >= 4 || data.playerOrder[order] > data.numConfPlayers) {
    data.currentPlayer = 5;
    return false;
  }
  data.currentPlayer = data.playerOrder[order];
  return true;
}

DescriptorHandle DatabaseImpl::getSavedRoomAddress() const
{
  uint16 addr = data.savedRoomDescriptorAddr;
  if (addr >= 0x1D0C)
    return addr - 0x1D0C;
  else
    return invalidHandle();
}

void DatabaseImpl::setSavedRoomAddress(DescriptorHandle room)
{
  if (room == ~0u)
    data.savedRoomDescriptorAddr = 0;
  else
    data.savedRoomDescriptorAddr = 0x1D0C + room;
  data.savedFloorDescriptorAddr =
    0x1D0C + (data.currentFloor-1) * data.descriptorBytesPerFloor;
}

void DatabaseImpl::clearRoomItemPosition(RoomItem n)
{
  data.roomItemPosition[n].row = 0;
  data.roomItemPosition[n].column = 0;
}

void DatabaseImpl::placeRoomItem(RoomItem n, byte y, byte x)
{
  data.roomItemPosition[n].row = 2*y + 1;
  data.roomItemPosition[n].column = 2*x + 3;
}

void DatabaseImpl::placeRoomItemCenter(RoomItem n)
{
  data.roomItemPosition[n].row = 8;
  data.roomItemPosition[n].column = 10;
}

void DatabaseImpl::copyRoomItemPosition(RoomItem n, RoomItem m)
{
  data.roomItemPosition[m].row = data.roomItemPosition[n].row;
  data.roomItemPosition[m].column = data.roomItemPosition[n].column;
}

Utils::StringSpan DatabaseImpl::getItemName(ItemCategory cat, byte id) const
{
  if (id--)
    switch(cat) {
    case ITEM_ARMORS:
      if (id < 8)
	return data.armors[id].name;
      /* FALLTHROUGH */
    case ITEM_SHIELDS:
      if (id >= 8) id -= 8;
      if (id < 6)
	return data.shields[id].name;
      break;
    case ITEM_WEAPONS:
      if (id < 8)
	return data.weapons[id].name;
      /* FALLTHROUGH */
    case ITEM_RANGED_WEAPONS:
      if (id >= 8) id -= 8;
      if (id < 8)
	return data.rangedWeapons[id].name;
      break;
    case ITEM_MAGIC_ITEMS:
      if (id & 0x80) {
	id = (byte(~id)-1) / 5;
	if (id < 8)
	  return data.magicItemCategories[id].name;
      } else if (id < 40)
	  return data.magicItems[id].name;
      break;
    case ITEM_QUEST_OBJECTS:
      if (id < 8)
	return data.questObjects[id].name;
      break;
    }
  return Utils::StringSpan();
}

Utils::StringSpan DatabaseImpl::getItemTiles(ItemCategory cat, byte id) const
{
  if (id--) {
    switch(cat) {
    case ITEM_ARMORS:
      if (id < 8)
	return data.objectTiles[5];
      /* FALLTHROUGH */
    case ITEM_SHIELDS:
      if (id >= 8) id -= 8;
      if (id < 6)
	return data.objectTiles[4];
      break;
    case ITEM_WEAPONS:
      if (id < 8)
	return data.objectTiles[2];
      /* FALLTHROUGH */
    case ITEM_RANGED_WEAPONS:
      if (id >= 8) id -= 8;
      if (id < 8)
	return data.objectTiles[3];
      break;
    case ITEM_MAGIC_ITEMS:
      if (id & 0x80)
	id = byte(~id)-1;
      id /= 5;
      if (id < 8)
	return data.magicItemCategories[id].tiles;
      break;
    case ITEM_QUEST_OBJECTS:
      if (id < 8)
	return data.questObjects[id].tiles;
      break;
    case ITEM_FLOOR_MAP:
      return data.objectTiles[8];
    }
  }
  return Utils::StringSpan();
}

int8 DatabaseImpl::getRangedWeaponAmmoType(unsigned id) const
{
  if (id > 8)
    id -= 8;
  if (id > 0 && id <= 8)
    return data.rangedWeapons[id-1].ammoType;
  else
    return 0;
}

Utils::StringSpan DatabaseImpl::getRangedWeaponAmmoName(unsigned id) const
{
  if (id > 8)
    id -= 8;
  if (id > 0 && id <= 8)
    return data.rangedWeapons[id-1].ammoName;
  else
    return Utils::StringSpan();
}

byte DatabaseImpl::getMagicItemInitialUses(byte n) const
{
  n = abs(n)-1;
  if (n < 40) {
    n = data.magicItems[n].usesLimit;
    if (abs(n) != n)
      n = abs(n);
    else
      n = randomSource->random(1, n);
  } else n = 0;
  return n;
}

void DatabaseImpl::setPlayerColor(unsigned n, unsigned c)
{
  static constexpr byte colors[] = { 0xce, 0x4e, 0xde, 0x6e };
  data.patternColors[n] = colors[c];
}

Utils::StringSpan DatabaseImpl::getFloorColorTable(unsigned floor) const
{
  if (floor != 0) {
    floor = (floor-1) >> 1;
    if (floor < 5)
      return data.corridorColors[floor];
  }
  return Utils::StringSpan();
}

Utils::StringSpan DatabaseImpl::getColorTable() const
{
  return data.patternColors;
}

Utils::StringSpan DatabaseImpl::getLivingStatueTiles() const { return data.objectTiles[1]; }
Utils::StringSpan DatabaseImpl::getFountainTiles() const { return data.objectTiles[0]; }
Utils::StringSpan DatabaseImpl::getChestTiles() const { return data.objectTiles[6]; }
Utils::StringSpan DatabaseImpl::getMoneyTiles() const { return data.objectTiles[7]; }

Utils::StringSpan DatabaseImpl::getExtDictionaryWord(byte n) const
{
  return data.extDictionary[n];
}

Utils::StringSpan DatabaseImpl::getDictionaryWord(byte n) const
{
  return data.dictionary[n];
}

void DatabaseImpl::getMagicEffectDescriptor(byte b, Base36Number (&effect)[3]) const
{
  unsigned n = b >> 2;
  if (n < 32) {
    effect[0] = Base36Number{data.magicEffect[n][0]};
    effect[1] = Base36Number{data.magicEffect[n][1]};
    effect[2] = Base36Number{data.magicEffect[n][2]};
  } else {
    effect[0] = Base36Number{' '};
    effect[1] = Base36Number{' '};
    effect[2] = Base36Number{' '};
  }
}

void DatabaseImpl::setFileData(bool isSave, unsigned len,
			       Utils::StringSpan name)
{
  enum {
    OPCODE_OPEN = 0,
    OPCODE_CLOSE = 1,
    OPCODE_READ = 2,
    OPCODE_WRITE = 3,
    OPCODE_REWIND = 4,
    OPCODE_LOAD = 5,
    OPCODE_SAVE = 6,
    OPCODE_DELETE = 7,
    OPCODE_SCRATCH = 8,
    OPCODE_STATUS = 9
  };
  enum {
    FLAG_FIX = 0x00,
    FLAG_VAR = 0x10,
    FLAG_DIS = 0x00,
    FLAG_INT = 0x08,
    FLAG_UPDATE = 0x00,
    FLAG_INPUT  = 0x04,
    FLAG_OUTPUT = 0x02,
    FLAG_APPEND = 0x06,
    FLAG_SEQ = 0x00,
    FLAG_REL = 0x01
  };
  data.pab[0] = (isSave? OPCODE_SAVE : OPCODE_LOAD);
  data.pab[1] = (isSave? (FLAG_FIX | FLAG_INT | FLAG_OUTPUT | FLAG_SEQ) :
		         (FLAG_FIX | FLAG_INT | FLAG_INPUT  | FLAG_SEQ));
  data.pab[2] = 0x04; // Load address: V@>0400
  data.pab[3] = 0x00;
  data.pab[4] = 0x80; // Record length
  data.pab[5] = 0x80; // Number of bytes
  data.pab[6] = 0x33; // File size: >3300
  data.pab[7] = 0x00;
  data.pab[8] = 0x00; // Screen offset
  data.pab[9] = len;  // DSR name length
  name.store(data.dsrname);
}

Utils::StringSpan DatabaseImpl::getFloorMap() const
{
  return data.floorMap;
}

void DatabaseImpl::clearRoomEnemies()
{
  Utils::clearArray(data.monsterName);
  data.monsterHPNumD6 = 0;
  data.monsterDefense = 0;
  data.monsterAttack = 0;
  data.monsterMaxDamage = 0;
  data.monsterSpecialAttackChance = 0;
  data.monsterSpecialAttackId = 0;
  data.unknown_1138 = 0;
  data.monsterSound = 0;
  data.monsterPatternNumber = 0;
  data.monsterNegotiation = 0;
  data.monsterMobility = 0;
  data.monsterResistance = 0;
  data.unknown_113e = 0;
  data.monsterSpeed = 0;
  Utils::clearArray(data.monsterHP);
  Utils::clearArray(data.monsterPosition, {0, 0});
  data.unknown_1156 = 0;
  data.unknown_1157 = 0;
  data.monsterInfo = 0;
}

void DatabaseImpl::prepareRoomEnemies(DescriptorHandle room)
{
  byte info = roomDescriptor(room)->monsterInfo;
  data.monsterInfo = info;
  data.unknown_115e = 0;
  unsigned n = data.unknown_1cfb * (data.currentFloor - 1) * 4 + (info & 0x1f);
  Utils::StringSpan{data.monsters[n].name}.store(data.monsterName);
  data.monsterHPNumD6 = data.monsters[n].hpNumD6;
  data.monsterDefense = data.monsters[n].defense;
  data.monsterAttack = data.monsters[n].attack;
  data.monsterMaxDamage = data.monsters[n].maxDamage;
  data.monsterSpecialAttackChance = data.monsters[n].specialAttackChance;
  data.monsterSpecialAttackId = data.monsters[n].specialAttackId;
  data.unknown_1138 = data.monsters[n].unknown1;
  data.unknown_113e = data.monsters[n].unknown4 & 0xf;
  data.monsterSpeed = data.monsters[n].unknown4 >> 4;
  data.monsterNegotiation = data.monsters[n].unknown3 >> 6;
  data.monsterMobility = ((data.monsters[n].unknown3 & 0x30) >> 4) + 1;
  data.monsterResistance = data.monsters[n].unknown3 & 0xf;
  data.monsterPatternNumber = data.monsters[n].unknown2 >> 4;
  data.monsterSound = data.monsters[n].unknown2 & 0xf;
  Utils::StringSpan patterns{data.monsterPatternTable[data.monsterPatternNumber]};
  patterns.store(data.patternTable, 240*8);
  patterns.store(data.patternTable, 244*8);
  Utils::clearArray(data.monsterHP);
}

int DatabaseImpl::getRoomNextLootSlot(DescriptorHandle room, unsigned &iterPos) const
{
  const RoomDescriptor *r = roomDescriptor(room);
  while (iterPos < 6) {
    byte c = byte(1) << iterPos;	
    if ((r->lootInfo & c)) {
      switch (iterPos) {
      case 0: c = byte(0) << 5; break;
      case 1: c = byte(2) << 5; break;
      default: c = byte(iterPos+2) << 5; break;
      }
      // byte mask = c; // Original code
      byte mask = ((c & 0x80)? 0xe0 : 0xc0); // Fixed code
      for (int i = 0; i < 3; i++)
	if (r->lootId[i] != 0 && (r->lootId[i] & mask) == c) {
	  ++iterPos;
	  return i;
	}
    }
    ++iterPos;
  }
  return -1;
}

void DatabaseImpl::clearRoomLootSlot(DescriptorHandle room, unsigned iterPos, unsigned n)
{
  RoomDescriptor *r = roomDescriptor(room);
  byte mask = byte(1) << (iterPos-1);
  r->lootInfo &= ~mask;
  if (n < 3)
    r->lootId[n] = 0;
}

bool DatabaseImpl::dropItemInRoom(DescriptorHandle room, ItemCategory cat, byte id, byte itemStat, byte itemAmmo)
{
  if (!id)
    return false;
  if (cat == ITEM_MAGIC_ITEMS && !(id & byte(0x80)))
    // Tested magical items can not be dropped
    return false;
  id = abs(id);
  if ((cat == ITEM_ARMORS || cat == ITEM_WEAPONS) && id > 8) 
    cat = ItemCategory(cat + 1);
  if ((cat == ITEM_SHIELDS || cat == ITEM_RANGED_WEAPONS) && id > 8)
    id -= 8;
  switch (cat) {
  case ITEM_ARMORS:
    if (id > 8)
      return false;
    if (!(itemStat >= data.armors[id-1].protection))
      // Dropping would improve stat
      return false;
    break;
  case ITEM_SHIELDS:
    if (id > 6)
      return false;
    if (!(itemStat >= data.shields[id-1].protection))
      // Dropping would improve stat
      return false;
    break;
  case ITEM_WEAPONS:
    if (id > 8)
      return false;
    if (!(itemStat >= data.weapons[id-1].damage))
      // Dropping would improve stat
      return false;
    break;
  case ITEM_RANGED_WEAPONS:
    if (id > 8)
      return false;
    if (!(itemStat >= data.rangedWeapons[id-1].damage))
      // Dropping would improve stat
      return false;
    if (data.rangedWeapons[id-1].ammoType == 1 ||
	data.rangedWeapons[id-1].ammoType == -1) {
      byte defAmmo = (data.rangedWeapons[id-1].defaultAmmo < 0?
		      -data.rangedWeapons[id-1].defaultAmmo :
		      data.rangedWeapons[id-1].defaultAmmo);
      if (defAmmo > itemAmmo)
	// Dropping would replenish ammo
	return false;
    }
    break;
  }

  RoomDescriptor *r = roomDescriptor(room);
  unsigned n;
  for (n=0; n<3; n++)
    if (!r->lootId[n])
      break;
  if (n >= 3)
    // No room for item in array
    return false;
  byte bit;
  switch (cat) {
  case ITEM_SHIELDS:
    id |= 8;
    /* FALLTHROUGH */
  case ITEM_ARMORS:
    bit = 4;
    break;
  case ITEM_RANGED_WEAPONS:
    id |= 8;
    /* FALLTHROUGH */
  case ITEM_WEAPONS:
    bit = 8;
    break;
  case ITEM_MAGIC_ITEMS:
    bit = 3; // Two concurrent items possible
    break;
  case ITEM_QUEST_OBJECTS:
    bit = 32;
    break;
  case ITEM_FLOOR_MAP:
    bit = 16;
    break;
  default:
    return false;
  }
  if ((r->lootInfo & bit) == bit)
    // Already at limit for this category
    return false;
  bit ^= (r->lootInfo & bit);
  if (bit == 3)
    bit = 1;
  r->lootInfo |= bit;
  byte c = 0x40;
  if (bit == 1)
    c = 0;
  else if (bit >= 4) do {
      c += 0x20;
    } while ((bit >>= 1) > 1);
  r->lootId[n] = id | c;
  return true;
}

byte DatabaseImpl::getRoomLootItem(DescriptorHandle room, unsigned n, ItemCategory &cat, byte &itemStat, byte &itemAmmo) const
{
  if (n >= 3)
    return 0;
  byte id = roomDescriptor(room)->lootId[n];
  itemStat = 0;
  itemAmmo = 0;
  if (id & 0x80) {
    switch (id >> 5) {
    case 4: cat = ITEM_ARMORS; break;
    case 5: cat = ITEM_WEAPONS; break;
    case 6: cat = ITEM_FLOOR_MAP; id=1; break;
    case 7: cat = ITEM_QUEST_OBJECTS; break;
    }
    id &= 0x1f;
    if (cat < ITEM_MAGIC_ITEMS && id > 8) {
      cat = ItemCategory(cat + 1);
      id -= 8;
    }
    switch(cat) {
    case ITEM_ARMORS:
      if (id < 8)
	itemStat = data.armors[id].protection;
      break;
    case ITEM_SHIELDS:
      if (id < 6)
	itemStat = data.shields[id].protection;
      break;
    case ITEM_WEAPONS:
      if (id < 8)
	itemStat = data.weapons[id].damage;
      break;
    case ITEM_RANGED_WEAPONS:
      if (id < 8) {
	itemStat = data.rangedWeapons[id].damage;
	byte defAmmo = (data.rangedWeapons[id].defaultAmmo < 0?
			-data.rangedWeapons[id].defaultAmmo :
			data.rangedWeapons[id].defaultAmmo);
	if (!(defAmmo >= data.unknown_1ce5))
	  defAmmo = data.unknown_1ce5;
	itemAmmo = defAmmo;
      }
      break;
    }
  } else {
    cat = ITEM_MAGIC_ITEMS;
    id = byte(~(id & 0x3f))+1;
  }
  return id;
}

void DatabaseImpl::setMapVisited(MapPosition pos, bool visited)
{
  uint16 p = PosWord(pos);
  if (p < sizeof(data.floorMap)/sizeof(data.floorMap[0]))
    if (visited)
      data.floorMap[p] &= ~0x10;
    else
      data.floorMap[p] |= 0x10;
}

void DatabaseImpl::clearMap(unsigned mode)
{
  for (unsigned pos = 0; pos < sizeof(data.floorMap)/sizeof(data.floorMap[0]);
       pos++)
    if (data.floorMap[pos] >= 0x60) {
      data.floorMap[pos] &= ~0x10;
      if (mode != 2) {
	if (data.floorMap[pos] == 0x69 && mode == 0) { // Descending stairs
	  data.floorMap[pos] = 0x68; // Change to ascending
	  continue;
	} else if (data.floorMap[pos] == 0x68 && mode == 1) // Ascending stairs
	  continue;
      }
      data.floorMap[pos] = 0x6b; // Clear to blank
    }
}

unsigned DatabaseImpl::findDescriptor(uint16 pos, unsigned offs, unsigned cnt,
				      unsigned delta) const
{
  offs += (data.currentFloor-1) * data.descriptorBytesPerFloor;
  while (cnt--) {
    if (offs >= sizeof(data.floorDescriptors)/sizeof(data.floorDescriptors[0])-1)
      break;
    if (pos == *reinterpret_cast<const msb16*>(data.floorDescriptors + offs))
      return offs;
    offs += delta;
  }
  return ~0u;
}

void DatabaseImpl::addMapFeatures(unsigned offs, unsigned cnt, byte code,
				  unsigned delta)
{
  while (cnt--) {
    if (offs >= sizeof(data.floorDescriptors)/sizeof(data.floorDescriptors[0])-1)
      break;
    uint16 pos = *reinterpret_cast<const msb16*>(data.floorDescriptors + offs);
    if (pos && pos < sizeof(data.floorMap)/sizeof(data.floorMap[0]))
      data.floorMap[pos] = code;
    offs += delta;
  }
}

unsigned DatabaseImpl::countBlankNeighbors(unsigned offs)
{
  unsigned n = 0;
  if (offs >= 32 && data.floorMap[offs-32] == 0x6b)
    n++;
  if (offs < sizeof(data.floorMap)/sizeof(data.floorMap[0])-32 &&
      data.floorMap[offs+32] == 0x6b)
    n++;
  if (offs >= 1 && data.floorMap[offs-1] == 0x6b)
    n++;
  if (offs < sizeof(data.floorMap)/sizeof(data.floorMap[0])-1 &&
      data.floorMap[offs+1] == 0x6b)
    n++;
  return n;
}

void DatabaseImpl::addMapVerticalCorridors()
{
  int x, y, last_y;
  bool candidate_room_seen;
  for (x=0; x<26; x++) {
    y = 0;
    last_y = -1;
    candidate_room_seen = false;
    while (y < 17) {
      unsigned offs = (y<<5)+x;
      byte code = data.floorMap[offs];
      if (code == 0x60)
	; /* Horizontal segment, allowed start/end of vertical segment */
      else if (code < 0x67 || code == 0x6b) {
	/* Other segment, or blank.  Not allowed start/end */
	y++;
	continue;
      } else {
	/* Room or fountain.  Allowed start/end, maybe candidate */
	if (countBlankNeighbors(offs) >= 3)
	  candidate_room_seen = true;
      }
      if (!candidate_room_seen || last_y < 0) {
	/* Don't complete the segment here, but keep it as a possible start */
	last_y = y++;
	continue;
      }
      /* New segment decided */
      for (int y2 = last_y; y2 <= y; y2++) {
	offs = (y2<<5)+x;
	code = data.floorMap[offs];
	if (code == 0x6b)
	  code = 0x61;
	else if (code == 0x63 && y2 != y)
	  code = 0x62;
	else if (code == 0x61)
	  break;
	else if (code == 0x60) {
	  if (y2 == last_y)
	    code = 0x64;
	  else if (y2 == y)
	    code = 0x63;
	  else
	    code = 0x62;
	}
	data.floorMap[offs] = code;
      }
      /* Reset state */
      last_y = -1;
      candidate_room_seen = false;
    }
  }
}

void DatabaseImpl::addMapHorizontalCorridors()
{
  int x, y, last_x;
  bool candidate_room_seen;
  for (y=0; y<17; y++) {
    x = 0;
    last_x = -1;
    candidate_room_seen = false;
    while (x < 26) {
      unsigned offs = (y<<5)+x;
      byte code = data.floorMap[offs];
      if (code == 0x61)
	; /* Vertical segment, allowed start/end of horizontal segment */
      else if (code < 0x67 || code == 0x6b) {
	/* Other segment, or blank.  Not allowed start/end */
	x++;
	continue;
      } else {
	/* Room or fountain.  Allowed start/end, maybe candidate */
	if (countBlankNeighbors(offs) >= 3)
	  candidate_room_seen = true;
      }
      if (!candidate_room_seen || last_x < 0) {
	/* Don't complete the segment here, but keep it as a possible start */
	last_x = x++;
	continue;
      }
      /* New segment decided */
      for (int x2 = last_x; x2 <= x; x2++) {
	offs = (y<<5)+x2;
	code = data.floorMap[offs];
	if (code == 0x6b)
	  code = 0x60;
	else if (code == 0x65 && x2 != x)
	  code = 0x62;
	else if (code == 0x60)
	  break;
	else if (code == 0x66 || code == 0x61) {
	  if (x2 == last_x)
	    code = 0x66;
	  else if (x2 == x)
	    code = 0x65;
	  else
	    code = 0x62;
	}
	data.floorMap[offs] = code;
      }
      /* Reset state */
      last_x = -1;
      candidate_room_seen = false;
    }
  }
}

void DatabaseImpl::fixupConnections()
{
  for (unsigned y=0; y<17; y++)
    for (unsigned x=0; x<26; x++) {
      unsigned offs = (y<<5)+x;
      byte code = data.floorMap[offs];
      if (code >= 0x68 && code <= 0x6a) {
	if (offs >= 32) {
	  code = data.floorMap[offs-32];
	  if (code == 0x60)
	    data.floorMap[offs-32] = 0x64;
	  else if (code == 0x63)
	    data.floorMap[offs-32] = 0x62;
	}
	if (offs < sizeof(data.floorMap)/sizeof(data.floorMap[0])-32) {
	  code = data.floorMap[offs+32];
	  if (code == 0x60)
	    data.floorMap[offs+32] = 0x63;
	  else if (code == 0x64)
	    data.floorMap[offs+32] = 0x62;
	}
	if (offs >= 1) {
	  code = data.floorMap[offs-1];
	  if (code == 0x61)
	    data.floorMap[offs-1] = 0x66;
	  else if (code == 0x65)
	    data.floorMap[offs-1] = 0x62;
	}
	if (offs < sizeof(data.floorMap)/sizeof(data.floorMap[0])-1) {
	  code = data.floorMap[offs+1];
	  if (code == 0x61)
	    data.floorMap[offs+1] = 0x64;
	  else if (code == 0x66)
	    data.floorMap[offs+1] = 0x62;
	}
      }
    }
}

void DatabaseImpl::prepareFloorMap(unsigned floor)
{
  clearMap(2);
  if (floor < 1 || floor > data.numFloors)
    return;
  uint16 floorBase = (floor-1) * data.descriptorBytesPerFloor;
  addMapFeatures(floorBase + 0, data.numRoomsPerFloor, 0x67, 10);
  addMapFeatures(floorBase + data.ascendingStairsDescriptorOffset,
		 data.numStairsPerFloor, 0x68, 2);
  addMapFeatures(floorBase + data.descendingStairsDescriptorOffset,
		 data.numStairsPerFloor, 0x69, 2);
  addMapFeatures(floorBase + data.fountainDescriptorOffset,
		 data.numFountainsPerFloor, 0x6a, 3);
  for (unsigned round = 0; round < 2; round++) {
    addMapVerticalCorridors();
    addMapHorizontalCorridors();
  }
  fixupConnections();
}

void DatabaseImpl::restoreFloorVisitedMarkers()
{
  for (uint16 p = 0; p < sizeof(data.floorMap)/sizeof(data.floorMap[0]); p++) {
    if (data.floorMap[p] >= 0x60)
      data.floorMap[p] |= 0x10;
    if (data.floorMap[p] == 0x77) {
      unsigned offs = findDescriptor(p);
      if (offs != ~0u &&
	  (reinterpret_cast<const RoomDescriptor*>
	   (&data.floorDescriptors[offs])->roomFlags & ROOM_FLAG_VISITED))
	data.floorMap[p] &= ~0x10;
    }
  }
}

Location DatabaseImpl::mapLocation(MapPosition pos) const
{
  uint16 p = PosWord(pos);
  if (p < sizeof(data.floorMap)/sizeof(data.floorMap[0])) {
    byte tile = data.floorMap[p] & ~0x10;
    if (tile >= 0x60 && tile < 0x6b) {
      if (tile < 0x67)
	return LOCATION_CORRIDOR;
      static constexpr Location locs[] = {
	LOCATION_ROOM, LOCATION_ASCENDING_STAIRCASE,
	LOCATION_DESCENDING_STAIRCASE, LOCATION_FOUNTAIN
      };
      return locs[tile-0x67];
    }
  }
  return LOCATION_BLANK;
}

bool DatabaseImpl::canMove(MapPosition pos, Direction dir, Location &dest) const
{
  unsigned blockages = 0;
  if (blockedForward(pos, dir))
    blockages++;
  pos.forward(dir);
  if (blockedBackward(pos, dir))
    blockages++;
  if ((dest = mapLocation(pos)) == LOCATION_BLANK)
    blockages = 2;
  pos.backward(dir);
  if (blockages == 1 && getSecretDoorsRevealed())
    blockages = 0;
  return !blockages;
}

bool DatabaseImpl::blockedForward(MapPosition pos, Direction dir) const
{
  uint16 p = PosWord(pos);
  if (p < sizeof(data.floorMap)/sizeof(data.floorMap[0])) {
    byte tile = data.floorMap[p] & ~0x10;
    if (tile >= 0x60 && tile < 0x6b) {
      if (tile >= 0x67)
	return false;
      static constexpr byte openings[] = {
	(1<<DIR_EAST) | (1<<DIR_WEST),
	(1<<DIR_NORTH) | (1<<DIR_SOUTH),
	(1<<DIR_NORTH) | (1<<DIR_EAST) | (1<<DIR_SOUTH) | (1<<DIR_WEST),
	(1<<DIR_NORTH) | (1<<DIR_EAST) | (1<<DIR_WEST),
	(1<<DIR_EAST) | (1<<DIR_SOUTH) | (1<<DIR_WEST),
	(1<<DIR_NORTH) | (1<<DIR_SOUTH) | (1<<DIR_WEST),
	(1<<DIR_NORTH) | (1<<DIR_EAST) | (1<<DIR_SOUTH),
      };
      return !(openings[tile-0x60] & (1<<dir));
    }
  }
  return true;
}

}}
