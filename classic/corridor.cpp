#include "system.h"

#include "classic/ScreenEngine.h"
#include "Utils.h"
#include "Database.h"
#include "FMTBuilder.h"

namespace Tunnels { namespace Classic {

void ScreenEngine::initCorridor()
{
  if (activePatternsAndColors == APAC_CORRIDOR)
    return;
  if (activePatternsAndColors == APAC_ROOM)
    screen.loadPatterns(128, database->getHighPatternTable(true));
  activePatternsAndColors = APAC_CORRIDOR;
  screen.loadColorTable(0, database->getColorTable());
  screen.setBackground(VDP::LIGHT_GREEN);
  Utils::StringSpan floorColors =
    database->getFloorColorTable(database->getCurrentFloor());
  if (floorColors) {
    screen.loadColorTable(16, floorColors);
    if (floorColors.len() < 11) {
      unsigned rpt = 11 - floorColors.len();
      floorColors.subspan(floorColors.len()-rpt, rpt);
      screen.loadColorTable(16+11-rpt, floorColors);
    }
  }
}

void ScreenEngine::clearMapOrCorridor()
{
  using namespace VDP::FMTBuilder;

  screen.all(' ');
  static constexpr const auto fmt1 =
    fmt(COL(2), ROW(1), RPTB(19, HCHA(28, 'k'), RIGHT(4)));
  fmt1(screen);
}

void ScreenEngine::drawCorridorSideWall(unsigned x0, unsigned x1)
{
  do {
    unsigned y;
    if (x0 > 15)
      screen.hchar((y = x0-10), x0, 0x83);
    else
      screen.hchar((y = 21-x0), x0, 0x82);
    int h = x0-15;
    if (h < 0)
      h = 1-h;
    h = 6 - (h>>1);
    if (h<=0)
      h = 1;
    if (h < y)
      do {
	if (!--y > 0)
	  break;
	screen.hchar(y, x0, 0x80);
      } while(y >= h);
  } while(++x0 <= x1);
}

void ScreenEngine::drawCorridorFloor(unsigned y0, unsigned y1)
{
  do {
    unsigned x = 21-y0+1;
    screen.hchar(y0, x, 0x81, (y0-6)<<1);
  } while(++y0 <= y1);
}

void ScreenEngine::corridorScreen()
{
  screen.clearSprite();
  screen.all(' ');
  initCorridor();
  clearMapOrCorridor();
}

void ScreenEngine::drawCorridorSegment(unsigned n, Location loc)
{
  using namespace VDP::FMTBuilder;

  if (n != 0 && loc != LOCATION_CORRIDOR && loc != LOCATION_FOUNTAIN) {
    switch(n) {
    case 1:
      if (loc == LOCATION_BLANK) {
	static constexpr const auto fmt1 =
	  fmt(COL(6), ROW(1), RPTB(15, HCHA(20, 0x90), RIGHT(12)));
	fmt1(screen);
      } else {
	static constexpr const auto fmt2 =
	  fmt(COL(6), ROW(1), RPTB(2, HCHA(20, 0x90), RIGHT(12)),
	      HCHA(6, 0x90), HCHA(8, 0xa6), HCHA(6, 0x90), RIGHT(12),
	      RPTB(11, HCHA(6, 0x90), HCHA(8, 0xa0), HCHA(6, 0x90), RIGHT(12)),
	      HCHA(6, 0x90), HCHA(8, 0xa7), HCHA(6, 0x90), RIGHT(12),
	      COL(19), ROW(9), HSTR("\xa3"));
	fmt2(screen);
      }
      break;
    case 2:
      static constexpr const auto fmt3 =
	fmt(COL(10), ROW(1), HCHA(12, 0xa8), RIGHT(20),
	    RPTB(2, HCHA(12, 0x90), RIGHT(20)));
      fmt3(screen);
      if (loc == LOCATION_BLANK) {
	static constexpr const auto fmt4 =
	  fmt(RPTB(8, HCHA(12, 0x90), RIGHT(20)));
	fmt4(screen);
      } else {
	static constexpr const auto fmt5 =
	  fmt(RPTB(8, HCHA(4, 0x90), HCHA(4, 0xa0), HCHA(4, 0x90), RIGHT(20)),
	      COL(17), ROW(8), HSTR("\xa3"));
	fmt5(screen);
      }
      break;
    case 3:
      static constexpr const auto fmt6 =
	fmt(COL(13), ROW(3), RPTB(2, HCHA(6, 0x90), RIGHT(26)));
      fmt6(screen);
      if (loc == LOCATION_BLANK) {
	static constexpr const auto fmt7 =
	  fmt(RPTB(4, HCHA(6, 0x90), RIGHT(26)));
	fmt7(screen);
      } else {
	static constexpr const auto fmt8 =
	  fmt(RPTB(4, HSTR("\x90\x90\xa0\xa0\x90\x90"), RIGHT(26)));
	fmt8(screen);
      }
      break;
    case 4:
      static constexpr const auto fmt9 =
	fmt(ROW(4), COL(15), HSTR("\x90\x90"), RIGHT(30));
      fmt9(screen);
      if (loc == LOCATION_BLANK) {
	static constexpr const auto fmt10 =
	  fmt(RPTB(2, HSTR("\x90\x90"), RIGHT(30)));
	fmt10(screen);
      } else {
	static constexpr const auto fmt11 =
	  fmt(RPTB(2, HSTR("\x91\x92"), RIGHT(30)));
	fmt11(screen);
      }
      break;
    }
  } else switch(n) {
    case 0:
      drawCorridorFloor(16, 19);
      static constexpr const auto fmt1 =
	fmt(COL(2), ROW(1), HCHA(4, 0x80), RIGHT(20), HCHA(4, 0x80));
      fmt1(screen);
      if (loc == LOCATION_FOUNTAIN) {
	static constexpr const auto fmt2 =
	  fmt(COL(11), ROW(16), HSTR("\xb2\xb0"), HCHA(6, 0xbb),
	      HSTR("\xb0"), HSTR("\xb3"), RIGHT(22), HSTR("\xb0\xb1"),
	      HSTR("\xb0\xb1\xb0\xb0\xb1\xb0\xb1\xb0"),
	      RIGHT(22), HSTR("\xb9"), HCHA(8, 0xb0), HSTR("\xba"),
	      RIGHT(22), HSTR("\xb8"), RIGHT(2), HCHA(4, 0xb8),
	      RIGHT(2), HSTR("\xb8"));
	fmt2(screen);
      }
      break;
    case 1:
      drawCorridorFloor(12, 15);
      drawCorridorSideWall(6, 6);
      drawCorridorSideWall(25, 25);
      static constexpr const auto fmt3 =
	fmt(COL(7), ROW(1), HSTR("\x80\x80\x89\x8a"),
	    HCHA(10, 0x88), HSTR("\x8c"), HSTR("\x8b"),
	    HCHA(2, 0x80), RIGHT(14), HCHA(4, 0x80), RIGHT(10), HCHA(4, 0x80));
      fmt3(screen);
      if (loc == LOCATION_FOUNTAIN) {
	static constexpr const auto fmt4 =
	  fmt(COL(13), ROW(12), HSTR("\xb2"), HCHA(4, 0xb0), HSTR("\xb3"),
	      RIGHT(26), HSTR("\xb9"), HCHA(4, 0xb0), HSTR("\xba"),
	      RIGHT(26), RPTB(2, HSTR("\xb8\x81\xb8")));
	fmt4(screen);
      }
      break;
    case 2:
      drawCorridorFloor(9, 11);
      static constexpr const auto fmt5 =
	fmt(COL(11), ROW(2), HSTR("\x89\x8a"), HCHA(6, 0x88),
	    HSTR("\x8c\x8b"));
      fmt5(screen);
      drawCorridorSideWall(10, 10);
      drawCorridorSideWall(21, 21);
      static constexpr const auto fmt6 =
	fmt(COL(11), ROW(3), HCHA(2, 0x80), RIGHT(6), HCHA(2, 0x80));
      fmt6(screen);
      if (loc == LOCATION_FOUNTAIN) {
	static constexpr const auto fmt7 =
	fmt(COL(14), ROW(9), HSTR("\xb2\xb0\xb0\xb3"),
	    RIGHT(29), HCHA(2, 0xb8));
	fmt7(screen);
      }
      break;
    case 3:
      drawCorridorFloor(7, 8);
      static constexpr const auto fmt8 =
	fmt(COL(13), ROW(3), HSTR("\x89\x8a\x88\x88\x8c\x8b"));
      fmt8(screen);
      drawCorridorSideWall(13, 13);
      drawCorridorSideWall(18, 18);
      if (loc == LOCATION_FOUNTAIN) {
	static constexpr const auto fmt9 =
	  fmt(COL(15), ROW(7), HSTR("\xb2\xb3"));
	fmt9(screen);
      }
      break;
    case 4:
      static constexpr const auto fmt10 =
	fmt(ROW(4), COL(15), HSTR("\x89\x8b"), RIGHT(30), HSTR("\x80\x80"),
	    RIGHT(30), HSTR("\x82\x83"));
      fmt10(screen);
      break;
    }
}

void ScreenEngine::drawCorridorLeftJunction(unsigned n, Location loc)
{
  using namespace VDP::FMTBuilder;

  switch(loc) {
  case LOCATION_BLANK:
    drawCorridorLeftWall(n);
    break;
  case LOCATION_CORRIDOR:
  case LOCATION_FOUNTAIN:
    switch(n) {
    case 0:
      static constexpr const auto fmt1 =
	fmt(COL(2), ROW(1), RPTB(15, HCHA(4, 0x90), RIGHT(28)),
	    RPTB(4, HCHA(4, 0x81), RIGHT(28)));
      fmt1(screen);
      break;
    case 1:
      static constexpr const auto fmt2 =
	fmt(COL(7), ROW(1), HCHA(3, 0xa8), RIGHT(29),
	    RPTB(10, HCHA(3, 0x90), RIGHT(29)),
	    RPTB(3, HCHA(3, 0x81), RIGHT(29)));
      fmt2(screen);
      break;
    case 2:
      static constexpr const auto fmt3 =
	fmt(COL(11), ROW(2), HCHA(2, 0x88), RIGHT(30),
	    RPTB(6, HCHA(2, 0x90), RIGHT(30)),
	    RPTB(2, HCHA(2, 0x81), RIGHT(30)));
      fmt3(screen);
      break;
    case 3:
      static constexpr const auto fmt4 =
	fmt(COL(14), ROW(3), VSTR("\x88\x90\x90\x90\x81"));
      fmt4(screen);
      break;
    }
    break;
  case LOCATION_ROOM:
  case LOCATION_DESCENDING_STAIRCASE:
  case LOCATION_ASCENDING_STAIRCASE:
    switch(n) {
    case 0:
      static constexpr const auto fmt5 =
	fmt(COL(2), ROW(2), VSTR("\x98"), VCHA(16, 0xa0), VSTR("\xc0"),
	    DOWN(6), RIGHT(1), VSTR("\x99"), VCHA(15, 0xa0), VSTR("\xc0"),
	    DOWN(7), RIGHT(1), VSTR("\x9a"), VCHA(14, 0xa0), VSTR("\xc0"),
	    DOWN(8), RIGHT(1), VSTR("\x9b\xa4"), VCHA(5, 0xa0), VSTR("\xa3"),
	    VCHA(6, 0xa0), VSTR("\xc0"));
	fmt5(screen);
      break;
    case 1:
      static constexpr const auto fmt6 =
	fmt(COL(7), ROW(3), VSTR("\x99"), VCHA(10, 0xa0), VSTR("\xc0"),
	    DOWN(12), RIGHT(1), VSTR("\x9a"), VCHA(9, 0xa0), VSTR("\xc0"),
	    DOWN(13), RIGHT(1), VSTR("\x9b\xa4"), VCHA(3, 0xa0), VSTR("\xa3"),
	    VCHA(3, 0xa0), VSTR("\xc0"));
	fmt6(screen);
      break;
    case 2:
      static constexpr const auto fmt7 =
	fmt(COL(11), ROW(4), VSTR("\x99"), VCHA(5, 0xa0), VSTR("\xc0"),
	    DOWN(17), RIGHT(1), VSTR("\x9a"), VCHA(4, 0xa0), VSTR("\xc0"));
      fmt7(screen);
      break;
    case 3:
      static constexpr const auto fmt8 =
	fmt(COL(14), ROW(4), VSTR("\x9b\xa4\xa0\xc0"));
      fmt8(screen);
      break;
    }
    break;
  }
}

void ScreenEngine::drawCorridorRightJunction(unsigned n, Location loc)
{
  using namespace VDP::FMTBuilder;

  switch(loc) {
  case LOCATION_BLANK:
    drawCorridorRightWall(n);
    break;
  case LOCATION_CORRIDOR:
  case LOCATION_FOUNTAIN:
    switch(n) {
    case 0:
      static constexpr const auto fmt1 =
	fmt(COL(26), ROW(1), RPTB(15, HCHA(4, 0x90), RIGHT(28)),
	    RPTB(4, HCHA(4, 0x81), RIGHT(28)));
      fmt1(screen);
      break;
    case 1:
      static constexpr const auto fmt2 =
	fmt(COL(22), ROW(1), HCHA(3, 0xa8), RIGHT(29),
	    RPTB(10, HCHA(3, 0x90), RIGHT(29)),
	    RPTB(3, HCHA(3, 0x81), RIGHT(29)));
      fmt2(screen);
      break;
    case 2:
      static constexpr const auto fmt3 =
	fmt(COL(19), ROW(2), HCHA(2, 0x88), RIGHT(30),
	    RPTB(6, HCHA(2, 0x90), RIGHT(30)),
	    RPTB(2, HCHA(2, 0x81), RIGHT(30)));
      fmt3(screen);
      break;
    case 3:
      static constexpr const auto fmt4 =
	fmt(COL(17), ROW(3), VSTR("\x88\x90\x90\x90\x81"));
      fmt4(screen);
      break;
    }
    break;
  case LOCATION_ROOM:
  case LOCATION_DESCENDING_STAIRCASE:
  case LOCATION_ASCENDING_STAIRCASE:
    switch(n) {
    case 0:
      static constexpr const auto fmt5 =
	fmt(COL(26), ROW(2), VSTR("\x9f\xa5"), VCHA(12, 0xa0), VSTR("\xc1"),
	    DOWN(9), RIGHT(1), VSTR("\x9e"), VCHA(14, 0xa0), VSTR("\xc1"),
	    DOWN(8), RIGHT(1), VSTR("\x9d"), VCHA(15, 0xa0), VSTR("\xc1"),
	    DOWN(7), RIGHT(1), VSTR("\x9c"), VCHA(8, 0xa0), VSTR("\xa3"),
	    VCHA(7, 0xa0), VSTR("\xc1"));
      fmt5(screen);
      break;
    case 1:
      static constexpr const auto fmt6 =
	fmt(COL(22), ROW(3), VSTR("\x9f\xa5"), VCHA(7, 0xa0), VSTR("\xc1"),
	    DOWN(14), RIGHT(1), VSTR("\x9e"), VCHA(9, 0xa0), VSTR("\xc1"),
	    DOWN(13), RIGHT(1), VSTR("\x9d"), VCHA(5, 0xa0), VSTR("\xa3"),
	    VCHA(4, 0xa0), VSTR("\xc1"));
      fmt6(screen);
      break;
    case 2:
      static constexpr const auto fmt7 =
	fmt(COL(19), ROW(4), VSTR("\x9e"), VCHA(4, 0xa0), VSTR("\xc1"),
	    DOWN(18), RIGHT(1), VSTR("\x9d"), VCHA(5, 0xa0), VSTR("\xc1"));
      fmt7(screen);
      break;
    case 3:
      static constexpr const auto fmt8 =
	fmt(COL(17), ROW(4), VSTR("\x9f\xa5\xa0\xc1"));
      fmt8(screen);
      break;
    }
    break;
  }
}

void ScreenEngine::drawCorridorLeftWall(unsigned n)
{
  switch(n) {
  case 0:
    drawCorridorSideWall(2, 5);
    break;
  case 1:
    drawCorridorSideWall(6, 9);
    break;
  case 2:
    drawCorridorSideWall(10, 12);
    break;
  case 3:
    drawCorridorSideWall(13, 14);
    break;
  }
}

void ScreenEngine::drawCorridorRightWall(unsigned n)
{
  switch(n) {
  case 0:
    drawCorridorSideWall(26, 29);
    break;
  case 1:
    drawCorridorSideWall(22, 25);
    break;
  case 2:
    drawCorridorSideWall(19, 21);
    break;
  case 3:
    drawCorridorSideWall(17, 18);
    break;
  }
}

void ScreenEngine::showCompass(Direction dir)
{
  screen.setSprite(8, 120, 0x80+(dir << 2), VDP::BLACK);
}

}}
