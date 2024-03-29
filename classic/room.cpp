#include "system.h"

#include "classic/ScreenEngine.h"
#include "Utils.h"
#include "Database.h"
#include "FMTBuilder.h"

namespace Tunnels { namespace Classic {

void ScreenEngine::mapScreen()
{
  clearMapOrCorridor();
  screen.hstr(2, 3, database->getFloorMap());
  drawPrompt(0x62);
  if (!database->hasHiddenMap() ||
      database->getMappedFloors() >= database->getCurrentFloor()) {
    static constexpr byte mapRevealColorTable[] { 0x4e, 0x4e };
    screen.loadColorTable(14, mapRevealColorTable);
  }
  MapPosition mapPos = database->getMapPosition();
  screen.setSprite((mapPos.y+2)*8-5, (mapPos.x+3)*8-4, 0x90, VDP::DARK_RED);
  screen.setYpt(23);
  screen.setXpt(2);
  drawPrompt(0x02);
}

void ScreenEngine::initRoom()
{
  if (activePatternsAndColors != APAC_ROOM) {
    screen.loadPatterns(128, database->getHighPatternTable(false));
    screen.setBackground(VDP::LIGHT_GREEN);
  }
  screen.loadColorTable(0, database->getColorTable());
  activePatternsAndColors = APAC_ROOM;
}

void ScreenEngine::clearMessages()
{
  using namespace VDP::FMTBuilder;

  // Clear message display
  if (screen.gchar(18, 2) < 0x60) screen.hchar(18, 2, ' ', 28);
  if (screen.gchar(19, 2) < 0x60) screen.hchar(19, 2, ' ', 28);
  // Clear bottom 4 lines
  static constexpr const auto fmt1 =
    fmt(ROW(20), COL(0), RPTB(4, RIGHT(2), HCHA(28, ' '), RIGHT(2)));
  fmt1(screen);
}

void ScreenEngine::clearVaultText()
{
  using namespace VDP::FMTBuilder;

  // Clear character sheet display / vault progress chart
  screen.setXpt(0);
  screen.setYpt(0);
  static constexpr const auto fmt1 =
    fmt(RPTB(18, RIGHT(20), HCHA(10, ' '), RIGHT(2)));
  fmt1(screen);
  clearMessages();
  vaultLine = 0;
}

void ScreenEngine::clearRoom()
{
  using namespace VDP::FMTBuilder;

  // Clear room map display
  screen.setXpt(0);
  screen.setYpt(0);
  static constexpr const auto fmt1 =
    fmt(RPTB(18, RIGHT(2), HCHA(18, 'k'), RIGHT(12)));
  fmt1(screen);
  clearVaultText();
}

void ScreenEngine::roomScreen()
{
  using namespace VDP::FMTBuilder;

  screen.setXpt(0);
  screen.setYpt(0);
  screen.clearSprite();
  screen.all(' ');
  initRoom();
  clearRoom();
  Location location = database->getCurrentLocation();
  MapPosition pos = database->getMapPosition();
  switch (location) {
  case LOCATION_ROOM:
  case LOCATION_DESCENDING_STAIRCASE:
  case LOCATION_ASCENDING_STAIRCASE:
  case LOCATION_ENTRANCE:
    // Room template
    static constexpr const auto fmt1 =
      fmt(COL(2), ROW(0),
	  RPTB(2, HCHA(5, 0xdd), RIGHT(8), HCHA(5, 0xdd), RIGHT(14)),
	  RPTB(2, RPTB(3, HCHA(2, 0xdd), RIGHT(14), HCHA(2, 0xdd), RIGHT(14)),
	       DOWN(8)),
	  DOWN(16), RPTB(2, HCHA(5, 0xdd), RIGHT(8), HCHA(5, 0xdd), RIGHT(14)),
	  COL(4), ROW(2), HSTR("\xdc"), HCHA(12, 0xd9), HSTR("\xdc"),
	  RPTB(12, RIGHT(18), HSTR("\xd8"), RIGHT(12), HSTR("\xd8")),
	  RIGHT(18), HSTR("\xdc"), HCHA(12, 0xd9), HSTR("\xdc"));
    fmt1(screen);
    if (location == LOCATION_ENTRANCE)
      return;
    // Add doors
    if (database->canMove(pos, DIR_NORTH)) {
      static constexpr const auto fmt2 =
	fmt(COL(9), ROW(2), HCHA(4, 0xdb));
      fmt2(screen);
    }
    if (database->canMove(pos, DIR_EAST)) {
      static constexpr const auto fmt3 =
	fmt(COL(17), ROW(7), VCHA(4, 0xda));
      fmt3(screen);
    }
    if (database->canMove(pos, DIR_SOUTH)) {
      static constexpr const auto fmt4 =
	fmt(COL(9), ROW(15), HCHA(4, 0xdb));
      fmt4(screen);
    }
    if (database->canMove(pos, DIR_WEST)) {
      static constexpr const auto fmt5 =
	fmt(COL(4), ROW(7), VCHA(4, 0xda));
      fmt5(screen);
    }
    break;
  case LOCATION_BLANK:
  case LOCATION_CORRIDOR:
  case LOCATION_FOUNTAIN:
    // Corridor encounter template
    static constexpr const auto fmt7 =
      fmt(COL(2), ROW(0),
	  RPTB(2, RPTB(5, HCHA(5, 0xdd), RIGHT(8), HCHA(5, 0xdd), RIGHT(14)),
	       DOWN(8)),
	  COL(6), ROW(4), RPTB(2, HSTR("\xdc"), RIGHT(8), HSTR("\xdc"),
			       RIGHT(22), DOWN(8)));
    fmt7(screen);
    Location nextLocation;
    if (location == LOCATION_ENTRANCE ||
	!database->canMove(pos, DIR_NORTH, nextLocation)) {
      static constexpr const auto fmt8 =
	fmt(COL(7), ROW(4), HCHA(8, 0xd9));
      fmt8(screen);
    } else if (nextLocation == LOCATION_ROOM ||
	       nextLocation > LOCATION_FOUNTAIN) {
      static constexpr const auto fmt9 =
	fmt(COL(7), ROW(4), HCHA(2, 0xd9), HCHA(4, 0xdb), HCHA(2, 0xd9));
      fmt9(screen);
    } else {
      static constexpr const auto fmt10 =
	fmt(COL(6), ROW(0), VCHA(4, 0xd8), DOWN(22), RIGHT(1),
	    HCHA(8, 0xdb), DOWN(22), VCHA(4, 0xd8));
      fmt10(screen);
    }
    if (location == LOCATION_ENTRANCE ||
	!database->canMove(pos, DIR_EAST, nextLocation)) {
      static constexpr const auto fmt11 =
	fmt(COL(15), ROW(5), VCHA(8, 0xd8));
      fmt11(screen);
    } else if (nextLocation == LOCATION_ROOM ||
	       nextLocation > LOCATION_FOUNTAIN) {
      static constexpr const auto fmt12 =
	fmt(COL(15), ROW(5), VCHA(2, 0xd8), VCHA(4, 0xda), VCHA(2, 0xd8));
      fmt12(screen);
    } else {
      static constexpr const auto fmt13 =
	fmt(ROW(4), COL(16), HCHA(4, 0xd9), RIGHT(29), VCHA(7, 0xda),
	    HSTR("\xda"), RIGHT(30), HCHA(4, 0xd9));
      fmt13(screen);
    }
    if (location == LOCATION_ENTRANCE ||
	!database->canMove(pos, DIR_SOUTH, nextLocation)) {
      static constexpr const auto fmt14 =
	fmt(COL(7), ROW(13), HCHA(8, 0xd9));
      fmt14(screen);
    } else if (nextLocation == LOCATION_ROOM ||
	       nextLocation > LOCATION_FOUNTAIN) {
      static constexpr const auto fmt15 =
	fmt(COL(7), ROW(13), HCHA(2, 0xd9), HCHA(4, 0xdb), HCHA(2, 0xd9));
      fmt15(screen);
    } else {
      static constexpr const auto fmt16 =
	fmt(COL(6), ROW(14), VCHA(4, 0xd8), DOWN(21), RIGHT(1),
	    HCHA(8, 0xdb), DOWN(23), VCHA(4, 0xd8));
      fmt16(screen);
    }
    if (location == LOCATION_ENTRANCE ||
	!database->canMove(pos, DIR_WEST, nextLocation)) {
      static constexpr const auto fmt17 =
	fmt(COL(6), ROW(5), VCHA(8, 0xd8));
      fmt17(screen);
    } else if (nextLocation == LOCATION_ROOM ||
	       nextLocation > LOCATION_FOUNTAIN) {
      static constexpr const auto fmt18 =
	fmt(COL(6), ROW(5), VCHA(2, 0xd8), VCHA(4, 0xda), VCHA(2, 0xd8));
      fmt18(screen);
    } else {
      static constexpr const auto fmt19 =
	fmt(ROW(4), COL(2), HCHA(4, 0xd9), RIGHT(30), VCHA(8, 0xda),
	    DOWN(23), RIGHT(30), HCHA(4, 0xd9));
      fmt19(screen);
    }
    
    break;
  }
}

void ScreenEngine::drawGameOver()
{
  using namespace VDP::FMTBuilder;

  static constexpr const auto fmt1 =
    fmt(COL(20), ROW(0), RPTB(19, HCHA(12, ' '), RIGHT(20)));
  fmt1(screen);
  drawPrompt(0x29);
}

void ScreenEngine::drawVault()
{
  using namespace VDP::FMTBuilder;

  static constexpr const auto fmt1 =
    fmt(COL(4), ROW(11), HSTR("\xdc\xd9\xdb\xd9"), VSTR("\xdc"),
	VCHA(3, 0xd8), HSTR("\xdc"));
  fmt1(screen);
}

bool ScreenEngine::isQuadBlocked(unsigned y, unsigned x)
{
  return screen.gchar(y, x) != 'k' || screen.gchar(y+1, x) != 'k' ||
    screen.gchar(y+1, x+1) != 'k' || screen.gchar(y, x+1) != 'k';
}

void ScreenEngine::putQuad(unsigned y, unsigned x, byte base)
{
  screen.hchar(y, x, base);
  screen.hchar(y+1, x, base+1);
  screen.hchar(y+1, x+1, base+3);
  screen.hchar(y, x+1, base+2);
}

void ScreenEngine::putQuad(unsigned y, unsigned x, Utils::StringSpan tiles)
{
  screen.hchar(y, x, tiles[0]);
  screen.hchar(y+1, x, tiles[1]);
  screen.hchar(y+1, x+1, tiles[3]);
  screen.hchar(y, x+1, tiles[2]);
}

void ScreenEngine::drawGeneralStore()
{
  putQuad(13, 5, 0x90);
}

void ScreenEngine::drawDynamicFixture()
{
  putQuad(database->getRoomItemRow(ROOM_ITEM_FIXTURE),
	  database->getRoomItemColumn(ROOM_ITEM_FIXTURE), 0xf4);
}

void ScreenEngine::drawLootItem(unsigned index, ItemCategory cat, byte id)
{
  putQuad(database->getRoomItemRow(RoomItem(ROOM_ITEM_LOOT1+index)),
	  database->getRoomItemColumn(RoomItem(ROOM_ITEM_LOOT1+index)),
	  database->getItemTiles(cat, id));
}

void ScreenEngine::clearLootItem(unsigned index)
{
  unsigned y = database->getRoomItemRow(RoomItem(ROOM_ITEM_LOOT1+index));
  unsigned x = database->getRoomItemColumn(RoomItem(ROOM_ITEM_LOOT1+index));
  screen.hchar(y, x, 'k', 2);
  screen.hchar(y+1, x, 'k', 2);
}

void ScreenEngine::drawLootItemName(ItemCategory cat, byte id)
{
  screen.hstr(18, 2, database->getItemName(cat, id));
}

void ScreenEngine::drawLivingStatue()
{
  putQuad(database->getRoomItemRow(ROOM_ITEM_CENTERPIECE),
	  database->getRoomItemColumn(ROOM_ITEM_CENTERPIECE),
	  database->getLivingStatueTiles());
}

void ScreenEngine::drawFountain()
{
  putQuad(database->getRoomItemRow(ROOM_ITEM_CENTERPIECE),
	  database->getRoomItemColumn(ROOM_ITEM_CENTERPIECE),
	  database->getFountainTiles());
}

void ScreenEngine::drawChestItem()
{
  putQuad(database->getRoomItemRow(ROOM_ITEM_CHEST),
	  database->getRoomItemColumn(ROOM_ITEM_CHEST),
	  database->getChestTiles());
}

void ScreenEngine::drawMoneyItem()
{
  putQuad(database->getRoomItemRow(ROOM_ITEM_MONEY),
	  database->getRoomItemColumn(ROOM_ITEM_MONEY),
	  database->getMoneyTiles());
}

bool ScreenEngine::isPlayerBlocked(unsigned n)
{
  return isQuadBlocked(database->getPlayerRow(n),
		       database->getPlayerColumn(n));
}

void ScreenEngine::drawPlayer(unsigned n)
{
  putQuad(database->getPlayerRow(n), database->getPlayerColumn(n), n<<3);
}

bool ScreenEngine::isMonsterBlocked(unsigned n)
{
  return isQuadBlocked(database->getMonsterRow(n),
		       database->getMonsterColumn(n));
}

void ScreenEngine::drawMonster(unsigned n)
{
  putQuad(database->getMonsterRow(n), database->getMonsterColumn(n), 0xf0);
}

void ScreenEngine::stairMovement(bool ascending)
{
  clearRoom();
  drawPrompt(ascending? 0x2f : 0x30);
}

bool ScreenEngine::checkIfRoomSquareOccupied(unsigned y, unsigned x)
{
  return isQuadBlocked(2*y+1, 2*x+3);
}

}}
