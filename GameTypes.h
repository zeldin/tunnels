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

constexpr Direction reverse(Direction d)
{
  return static_cast<Direction>((d+2)&3);
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

}

#endif // TUNNELS_GAMETYPES_H_
