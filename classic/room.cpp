#include "system.h"

#include "classic/ScreenEngine.h"
#include "Utils.h"
#include "Database.h"

namespace Tunnels { namespace Classic {

void ScreenEngine::initRoom()
{
  if (activePatternTable != 1) {
    screen.loadPatterns(128, database->getHighPatternTable(false));
    screen.setBackground(VDP::LIGHT_GREEN);
    activePatternTable = 1;
  }
  screen.loadColorTable(0, database->getColorTable());
}

void ScreenEngine::clearRoom()
{
  // Clear room map display
  screen.setXpt(0);
  screen.setYpt(0);
  static constexpr byte fmt1[] = {
    0xd1, 0x81, 0x51, 0x6b, 0x8b, 0xfb, 0x00, 0x01, 0xfb,
  };
  screen.fmt(fmt1);
  // Clear character sheet display
  screen.setXpt(0);
  screen.setYpt(0);
  static constexpr byte fmt2[] = {
    0xd1, 0x93, 0x49, 0x20, 0x81, 0xfb, 0x00, 0x01, 0xfb,
  };
  screen.fmt(fmt2);
  // Clear message display
  if (screen.gchar(18, 2) < 0x60) screen.hchar(18, 2, ' ', 28);
  if (screen.gchar(19, 2) < 0x60) screen.hchar(19, 2, ' ', 28);
  // Clear current player indicator
  static constexpr byte fmt3[] = {
    0xfe, 0x14, 0xff, 0x00, 0xc3, 0x81, 0x5b, 0x20,
    0x81, 0xfb, 0x00, 0x05, 0xfb,
  };
  screen.fmt(fmt3);
}

void ScreenEngine::roomScreen()
{
  screen.setXpt(0);
  screen.setYpt(0);
  // FIXME: Kill sprites
  screen.all(' ');
  initRoom();
  clearRoom();
  Database::Location location = database->getCurrentLocation();
  Database::MapPosition pos = database->getMapPosition();
  switch (location) {
  case Database::LOCATION_ROOM:
  case Database::LOCATION_DESCENDING_STAIRCASE:
  case Database::LOCATION_ASCENDING_STAIRCASE:
  case Database::LOCATION_ENTRANCE:
    // Room template
    static constexpr byte fmt1[] = {
      0xff, 0x02, 0xfe, 0x00, 0xc1, 0x44, 0xdd, 0x87,
      0x44, 0xdd, 0x8d, 0xfb, 0x00, 0x05, 0xc1, 0xc2,
      0x41, 0xdd, 0x8d, 0x41, 0xdd, 0x8d, 0xfb, 0x00,
      0x10, 0xa7, 0xfb, 0x00, 0x0f, 0xaf, 0xc1, 0x44,
      0xdd, 0x87, 0x44, 0xdd, 0x8d, 0xfb, 0x00, 0x1f,
      0xff, 0x04, 0xfe, 0x02, 0x00, 0xdc, 0x4b, 0xd9,
      0x00, 0xdc, 0xcb, 0x91, 0x00, 0xd8, 0x8b, 0x00,
      0xd8, 0xfb, 0x00, 0x33, 0x91, 0x00, 0xdc, 0x4b,
      0xd9, 0x00, 0xdc, 0xfb,
    };
    screen.fmt(fmt1);
    if (location == Database::LOCATION_ENTRANCE)
      return;
    // Add doors
    if (database->canMove(pos, Database::DIR_NORTH)) {
      static constexpr byte fmt2[] = {
	0xff, 0x09, 0xfe, 0x02, 0x43, 0xdb, 0xfb,
      };
      screen.fmt(fmt2);
    }
    if (database->canMove(pos, Database::DIR_EAST)) {
      static constexpr byte fmt3[] = {
	0xff, 0x11, 0xfe, 0x07, 0x63, 0xda, 0xfb,
      };
      screen.fmt(fmt3);
    }
    if (database->canMove(pos, Database::DIR_SOUTH)) {
      static constexpr byte fmt4[] = {
	0xff, 0x09, 0xfe, 0x0f, 0x43, 0xdb, 0xfb,
      };
      screen.fmt(fmt4);
    }
    if (database->canMove(pos, Database::DIR_WEST)) {
      static constexpr byte fmt5[] = {
	0xff, 0x04, 0xfe, 0x07, 0x63, 0xda, 0xfb,
      };
      screen.fmt(fmt5);
    }
    if (location != Database::LOCATION_ROOM)
      return;
    if (false /* FIXME */) {
      // Add vault
      static constexpr byte fmt6[] = {
	0xff, 0x04, 0xfe, 0x0b, 0x03, 0xdc, 0xd9, 0xdb,
	0xd9, 0x20, 0xdc, 0x62, 0xd8, 0x00, 0xdc, 0xfb,
      };
      screen.fmt(fmt6);
    }
    break;
  case Database::LOCATION_BLANK:
  case Database::LOCATION_CORRIDOR:
  case Database::LOCATION_FOUNTAIN:
    // Corridor encounter template
    static constexpr byte fmt7[] = {
      0xff, 0x02, 0xfe, 0x00, 0xc1, 0xc4, 0x44, 0xdd,
      0x87, 0x44, 0xdd, 0x8d, 0xfb, 0x00, 0x06, 0xa7,
      0xfb, 0x00, 0x05, 0xff, 0x06, 0xfe, 0x04, 0xc1,
      0x00, 0xdc, 0x87, 0x00, 0xdc, 0x95, 0xa7, 0xfb,
      0x00, 0x18, 0xfb,
    };
    screen.fmt(fmt7);
    Database::Location nextLocation;
    if (location == Database::LOCATION_ENTRANCE ||
	!database->canMove(pos, Database::DIR_NORTH, nextLocation)) {
      static constexpr byte fmt8[] = {
	0xff, 0x07, 0xfe, 0x04, 0x47, 0xd9, 0xfb,
      };
      screen.fmt(fmt8);
    } else if (nextLocation == Database::LOCATION_ROOM ||
	       nextLocation > Database::LOCATION_FOUNTAIN) {
      static constexpr byte fmt9[] = {
	0xff, 0x07, 0xfe, 0x04, 0x41, 0xd9, 0x43, 0xdb,
	0x41, 0xd9, 0xfb,
      };
      screen.fmt(fmt9);
    } else {
      static constexpr byte fmt10[] = {
	0xff, 0x06, 0xfe, 0x00, 0x63, 0xd8, 0xb5, 0x80,
	0x47, 0xdb, 0xb5, 0x63, 0xd8, 0xfb,
      };
      screen.fmt(fmt10);
    }
    if (location == Database::LOCATION_ENTRANCE ||
	!database->canMove(pos, Database::DIR_EAST, nextLocation)) {
      static constexpr byte fmt11[] = {
	0xff, 0x0f, 0xfe, 0x05, 0x67, 0xd8, 0xfb,
      };
      screen.fmt(fmt11);
    } else if (nextLocation == Database::LOCATION_ROOM ||
	       nextLocation > Database::LOCATION_FOUNTAIN) {
      static constexpr byte fmt12[] = {
	0xff, 0x0f, 0xfe, 0x05, 0x61, 0xd8, 0x63, 0xda,
	0x61, 0xd8, 0xfb,
      };
      screen.fmt(fmt12);
    } else {
      static constexpr byte fmt13[] = {
	0xfe, 0x04, 0xff, 0x10, 0x43, 0xd9, 0x9c, 0x66,
	0xda, 0x00, 0xda, 0x9d, 0x43, 0xd9, 0xfb,
      };
      screen.fmt(fmt13);
    }
    if (location == Database::LOCATION_ENTRANCE ||
	!database->canMove(pos, Database::DIR_SOUTH, nextLocation)) {
      static constexpr byte fmt14[] = {
	0xff, 0x07, 0xfe, 0x0d, 0x47, 0xd9, 0xfb,
      };
      screen.fmt(fmt14);
    } else if (nextLocation == Database::LOCATION_ROOM ||
	       nextLocation > Database::LOCATION_FOUNTAIN) {
      static constexpr byte fmt15[] = {
	0xff, 0x07, 0xfe, 0x0d, 0x41, 0xd9, 0x43, 0xdb,
	0x41, 0xd9, 0xfb,
      };
      screen.fmt(fmt15);
    } else {
      static constexpr byte fmt16[] = {
	0xff, 0x06, 0xfe, 0x0e, 0x63, 0xd8, 0xb4, 0x80,
	0x47, 0xdb, 0xb6, 0x63, 0xd8, 0xfb,
      };
      screen.fmt(fmt16);
    }
    if (location == Database::LOCATION_ENTRANCE ||
	!database->canMove(pos, Database::DIR_WEST, nextLocation)) {
      static constexpr byte fmt17[] = {
	0xff, 0x06, 0xfe, 0x05, 0x67, 0xd8, 0xfb,
      };
      screen.fmt(fmt17);
    } else if (nextLocation == Database::LOCATION_ROOM ||
	       nextLocation > Database::LOCATION_FOUNTAIN) {
      static constexpr byte fmt18[] = {
	0xff, 0x06, 0xfe, 0x05, 0x61, 0xd8, 0x63, 0xda,
	0x61, 0xd8, 0xfb,
      };
      screen.fmt(fmt18);
    } else {
      static constexpr byte fmt19[] = {
	0xfe, 0x04, 0xff, 0x02, 0x43, 0xd9, 0x9d, 0x67,
	0xda, 0xb6, 0x9d, 0x43, 0xd9, 0xfb,
      };
      screen.fmt(fmt19);
    }
    
    break;
  }
}

void ScreenEngine::drawGeneralStore()
{
  putQuad(13, 5, 0x90);
}

void ScreenEngine::drawStaircase()
{
  putQuad(13, 15, 0xf4);
}

}}
