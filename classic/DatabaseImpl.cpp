#include "system.h"

#include "classic/DatabaseImpl.h"
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
  static constexpr byte zeroes[8*8] = {0,};
  Utils::StringSpan(zeroes).store(data.patternTable, 240*8);
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

void DatabaseImpl::setPlayerClass(unsigned n, unsigned c)
{
  data.player[n].flags = c << 6;
  for (unsigned i = 0; i < 64; i++)
    data.patternTable[(n << 6) | i] = data.classPatternTable[c][i];
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

Utils::StringSpan DatabaseImpl::getClassName(unsigned n) const
{
  return data.classes[n].name;
}

Utils::StringSpan DatabaseImpl::getClassPatternTable(unsigned n) const
{
  return data.classPatternTable[n];
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

Utils::StringSpan DatabaseImpl::getItemName(ItemCategory cat, byte id) const
{
  if (id--)
    switch(cat) {
    case ITEM_ARMORS:
      if (id < 8)
	return data.armors[id].name;
      id -= 8;
      /* FALLTHROUGH */
    case ITEM_SHIELDS:
      if (id < 6)
	return data.shields[id].name;
      break;
    case ITEM_WEAPONS:
      if (id < 8)
	return data.weapons[id].name;
      id -= 8;
      /* FALLTHROUGH */
    case ITEM_RANGED_WEAPONS:
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

int8 DatabaseImpl::getRangedWeaponAmmoType(unsigned id) const
{
  if (id > 8)
    id -= 8;
  if (id > 0 && id <= 6)
    return data.rangedWeapons[id-1].ammoType;
  else
    return 0;
}

Utils::StringSpan DatabaseImpl::getRangedWeaponAmmoName(unsigned id) const
{
  if (id > 8)
    id -= 8;
  if (id > 0 && id <= 6)
    return data.rangedWeapons[id-1].ammoName;
  else
    return Utils::StringSpan();
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

void DatabaseImpl::setMapVisited(MapPosition pos, bool visited)
{
  uint16 p = PosWord(pos);
  if (p < sizeof(data.floorMap)/sizeof(data.floorMap[0]))
    if (visited)
      data.floorMap[p] &= ~0x10;
    else
      data.floorMap[p] |= 0x10;
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
