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
  DIR_WEST = 3
};

enum StartPosition {
  STARTPOS_NORMAL = 0,
  STARTPOS_BACK_AGAINST_WALL = 1,
  STARTPOS_IN_DOORWAY = 2,
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
