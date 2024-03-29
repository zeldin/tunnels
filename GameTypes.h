#ifndef TUNNELS_GAMETYPES_H_
#define TUNNELS_GAMETYPES_H_

namespace Tunnels {

enum Location {
  LOCATION_BLANK = 0,
  LOCATION_CORRIDOR = 1,
  LOCATION_ROOM = 2,
  LOCATION_FOUNTAIN = 3,
  LOCATION_DESCENDING_STAIRCASE = 4,
  LOCATION_ASCENDING_STAIRCASE = 5,
  LOCATION_ENTRANCE = 6
};

enum Direction {
  DIR_NORTH = 0,
  DIR_EAST = 1,
  DIR_SOUTH = 2,
  DIR_WEST = 3,
  DIR_NONE = 0xff
};

enum StartPosition {
  STARTPOS_NORMAL = 0,
  STARTPOS_BACK_AGAINST_WALL = 1,
  STARTPOS_IN_DOORWAY = 2,
};

enum KeyMapping {
  KEYMAP_BREAK_DOOR = 0,
  KEYMAP_CHECK_HIDDEN_DOORS = 1,
  KEYMAP_FIRE_RANGED_WEAPON = 2,
  KEYMAP_CHANGE_WEAPON = 3,
  KEYMAP_LISTEN_AT_DOOR = 4,
  KEYMAP_LOOK_AT_MAP = 5,
  KEYMAP_CHANGE_ORDER = 6,
  KEYMAP_SAVE_GAME = 7,
  KEYMAP_TRADE_ITEMS = 8,
  KEYMAP_USE_ITEM = 9,
  KEYMAP_ATTEMPT_NEGOTIATION = 10
};

enum ItemCategory {
  ITEM_ARMORS = 0,
  ITEM_SHIELDS = 1,
  ITEM_WEAPONS = 2,
  ITEM_RANGED_WEAPONS = 3,
  ITEM_MAGIC_ITEMS = 4,
  ITEM_QUEST_OBJECTS = 5,
  ITEM_FLOOR_MAP = 6 // Not part of original enum...
};

enum RoomFixture {
  FIXTURE_FOUNTAIN = 0,
  FIXTURE_LIVING_STATUE = 1,
  FIXTURE_ASCENDING_STAIRS = 2,
  FIXTURE_DESCENDING_STAIRS = 3,
  FIXTURE_GENERAL_STORE = 4,
  FIXTURE_VAULT = 5
};

enum RoomItem {
  ROOM_ITEM_FIXTURE = 0, // If special room (general store / vault)
  ROOM_ITEM_CHEST = 0, // If normal room
  ROOM_ITEM_MONEY = 1,
  ROOM_ITEM_CENTERPIECE = 2, // Fountain / living statue
  ROOM_ITEM_LOOT1 = 3,
  ROOM_ITEM_LOOT2 = 4,
  ROOM_ITEM_LOOT3 = 5,

  ROOM_ITEM_FIRST = ROOM_ITEM_CHEST,
  ROOM_ITEM_LAST = ROOM_ITEM_LOOT3
};

enum ExtDictionaryWord {
  EXT_DICTIONARY_FOUNTAIN = 0,
  EXT_DICTIONARY_LIVING_STATUE = 1,
  EXT_DICTIONARY_CURRENCY = 2,
  EXT_DICTIONARY_ITEMS = 3,
  EXT_DICTIONARY_CHEST = 4,
  EXT_DICTIONARY_VAULT = 5,
  EXT_DICTIONARY_COMBINATION = 6,
  EXT_DICTIONARY_OPEN = 7,
  EXT_DICTIONARY_COMBINATION_FOUND = 8,
  EXT_DICTIONARY_GENERAL_STORE = 9
};

constexpr RoomItem next(RoomItem i)
{
  return RoomItem(i + 1);
}


constexpr Direction operator+(Direction a, Direction b)
{
  return static_cast<Direction>((unsigned(a)+unsigned(b))&3);
}

constexpr Direction reverse(Direction d)
{
  return d + DIR_SOUTH;
}

constexpr Direction left(Direction d)
{
  return d + DIR_WEST;
}

constexpr Direction right(Direction d)
{
  return d + DIR_EAST;
}

struct MapPosition {
  byte x;
  byte y;
  void north() { --y; }
  void east() { x++; }
  void south() { y++; }
  void west() { --x; }
  void forward(Direction d) {
    switch(d) {
    case DIR_NORTH: north(); break;
    case DIR_EAST: east(); break;
    case DIR_SOUTH: south(); break;
    case DIR_WEST: west(); break;
    }
  }
  void backward(Direction d) { forward(reverse(d)); }
};

struct Base36Number {
  byte n;
  operator bool() const { return n != ' '; }
  operator byte() const {
    byte r = n-'0';
    if (r >= 10)
      r -= 7;
    return (r < 36? r : 0);
  };
};

typedef unsigned DescriptorHandle;
constexpr DescriptorHandle invalidHandle() { return ~0u; }

}

#endif // TUNNELS_GAMETYPES_H_
