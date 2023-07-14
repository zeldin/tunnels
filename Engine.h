#ifndef TUNNELS_ENGINE_H_
#define TUNNELS_ENGINE_H_

#include "EventLoop.h"
#include "GameTypes.h"

namespace Tunnels {

namespace Timer { class TimerManager; }
namespace Utils { class StringSpan; }
namespace File { class Backend; }

class Database;
class DatabaseFactory;

class ScreenEngine {
public:
  virtual ~ScreenEngine() {}
  virtual void refresh() {}
  virtual void setDatabase(const Database *database) {}
  virtual void drawTitleScreen() = 0;
  virtual void initMenu() = 0;
  virtual void drawPrompt(unsigned n, unsigned param) = 0;
  virtual void setCursorEnabled(bool enabled) = 0;
  virtual void setBorderFlashEnabled(bool enabled) = 0;
  virtual void markSelection(byte ch) = 0;
  virtual void prepareStringInputField(unsigned len) = 0;
  virtual void endStringInputField(unsigned len, unsigned cnt) = 0;
  virtual void addStringInputField(byte ch) = 0;
  virtual void eraseStringInputField(unsigned cnt) = 0;
  virtual void drawIoError(bool casette, byte error) = 0;
  virtual void preparePlayerNameInput(unsigned n) = 0;
  virtual void preparePlayerClassInput() = 0;
  virtual void preparePlayerColorInput() = 0;
  virtual void prepareItemNumberInput() = 0;
  virtual void askCharacterAccept() {};
  virtual void setPlayerColors() {}
  virtual void setPlayerShapes(unsigned n) {}
  virtual void setRoomFixtureShapes() {}
  virtual void roomScreen() = 0;
  virtual void corridorScreen() = 0;
  virtual void mapScreen() = 0;
  virtual void clearMessages() = 0;
  virtual void drawGameOver() = 0;
  virtual void drawVault() = 0;
  virtual void drawGeneralStore() = 0;
  virtual void drawDynamicFixture() = 0;
  virtual void drawLivingStatue() = 0;
  virtual void drawFountain() = 0;
  virtual void drawChestItem() = 0;
  virtual void drawMoneyItem() = 0;
  virtual void drawLootItem(unsigned index, ItemCategory cat, byte id) = 0;
  virtual void clearLootItem(unsigned index) = 0;
  virtual void drawLootItemName(ItemCategory cat, byte id) = 0;
  virtual void stairMovement(bool ascending) = 0;
  virtual void drawCorridorSegment(unsigned n, Location loc) = 0;
  virtual void drawCorridorLeftJunction(unsigned n, Location loc) = 0;
  virtual void drawCorridorRightJunction(unsigned n, Location loc) = 0;
  virtual void drawCorridorLeftWall(unsigned n) = 0;
  virtual void drawCorridorRightWall(unsigned n) = 0;
  virtual void drawDoorReveal(unsigned n) = 0;
  virtual void showCompass(Direction dir) = 0;
  virtual void drawPlayer(unsigned n) = 0;
  virtual bool isPlayerBlocked(unsigned n) = 0;
  virtual void drawMonster(unsigned n) = 0;
  virtual bool isMonsterBlocked(unsigned n) = 0;
  virtual void drawPlayerStatusHeader(unsigned n) = 0;
  virtual void drawMagicEffectDescription(byte id) = 0;
  virtual void drawMagicItemDescription(unsigned id) = 0;
  virtual bool checkIfRoomSquareOccupied(unsigned y, unsigned x) = 0;
  void drawPrompt(unsigned n) { drawPrompt(n, 0); }
};

class SoundEngine {
public:
  virtual ~SoundEngine() {}
  virtual void stopMusic() = 0;
  virtual void playTitleMusic() = 0;
  virtual void playStairMusic() = 0;
  virtual void playGeneralStoreMusic() = 0;
  virtual void playMonsterSound(unsigned n) = 0;
  virtual void playQuestObjectFailedMusic() = 0;
  virtual void playQuestObjectCompleteMusic() = 0;
  virtual void beep() = 0;
  virtual void honk() = 0;
};

class GameEngine {
private:
  EventLoop &eventLoop;
  Timer::TimerManager &timerManager;
  ScreenEngine &screen;
  SoundEngine &sound;
  File::Backend &file;
  DatabaseFactory &databaseFactory;
  Database *database;

  enum Diversion {
    DIVERSION_NULL,           // keep going
    DIVERSION_LOAD_SAVE,      // G@>6046
    DIVERSION_LOAD_SAVE_BACK, // G@>6108
    DIVERSION_NEW_OR_RESTOCK, // G@>6114
    DIVERSION_CREATE_PARTY,   // G@>61DD
    DIVERSION_POINT_OF_NR,    // G@>62D1
    DIVERSION_BUILD_DUNGEON,  // G@>62F4
    DIVERSION_STAIRCASE,      // G@>64F1
    DIVERSION_ENTRANCE,       // G@>6538
    DIVERSION_ENTER_ROOM,     // G@>654C
    DIVERSION_ENTER_LOCATION, // G@>6551
    DIVERSION_CONTINUE_GAME,  // G@>6556
    DIVERSION_ROOM_MAIN,      // G@>65B2
    DIVERSION_CORRIDOR,       // G@>66F7
    DIVERSION_CORRIDOR_MAIN,  // G@>670B
    DIVERSION_LOOT_ROOM,      // G@>C41F
    DIVERSION_AID,
    DIVERSION_UP,
    DIVERSION_DOWN,
    DIVERSION_REDO,
    DIVERSION_PROCD,
    DIVERSION_BACK,
    DIVERSION_QUIT
  };
  Diversion redoTarget;
  Diversion backTarget;
  Diversion procdTarget;

  enum {
    ACCEPT_PROCD         = 0x01,
    ACCEPT_BACK          = 0x02,
    ACCEPT_REDO          = 0x04,
    ACCEPT_AID           = 0x08,
    ACCEPT_UP_DOWN       = 0x10,
    ACCEPT_NUMERIC       = 0x20,
    ACCEPT_ALPHANUMERIC  = 0x40,
    ACCEPT_ALPHANUMERIC2 = 0x80
  };
  byte acceptMask;

  Direction direction;
  DescriptorHandle currentRoom;
  byte lastActionKey;
  byte progression;
  uint16 randState;
  int8 roomDone;

public:
  GameEngine(EventLoop &eventLoop_, Timer::TimerManager &timerManager_,
	     ScreenEngine &screen_, SoundEngine &sound_, File::Backend &file_,
	     DatabaseFactory &databaseFactory_);
  EventType run();

private:
  byte random(byte upper);
  byte random(byte lower, byte upper);
  Event nextEvent();
  EventType waitForEvent();
  Diversion delay(unsigned ms);
  Diversion waitForMusic();
  void initEnemyHealth();
  void pickUnoccupiedRoomSquare(byte &y, byte &x);
  void pickItemRoomSquare(byte &y, byte &x);
  void setRoomFixtureShape(RoomFixture f);
  Diversion core(bool newLocation);
  Diversion staircase();
  Diversion entrance();
  void placeRoomItems();
  void drawLoot();
  Diversion lootRoom();
  void roomSetup(bool newLocation);
  Diversion room();
  Diversion corridor();
  bool tryMove(bool checkOnly = false);
  Diversion getMovementKey(byte &kc, Direction &dir);
  Diversion titleScreen();
  Diversion loadSaveMenu();
  Diversion loadSave(bool isSave, unsigned len, Utils::StringSpan name);
  void preserveState();
  void restoreState();
  Diversion newOrRestockMenu();
  Diversion createPartyMenu();
  Diversion createPartyMember(unsigned player);
  Diversion pointOfNoReturnMenu();
  Diversion getKeyNoCursor(byte &kc);
  Diversion getKey(byte &kc);
  Diversion getNumberKey(byte &n, byte low, byte high);
  Diversion getString(unsigned len, byte *result = nullptr);
  template<unsigned n> Diversion getString(byte (&result)[n])
  { return getString(n, &result[0]); }
  Diversion getNumber(unsigned lower, unsigned upper, unsigned &value,
		      bool x10=false);
  void ioError(bool casette, byte error);
  Diversion flashBorder();
};

}

#endif // TUNNELS_ENGINE_H_
