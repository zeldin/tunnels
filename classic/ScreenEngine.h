#ifndef TUNNELS_CLASSIC_SCREENENGINE_H_
#define TUNNELS_CLASSIC_SCREENENGINE_H_

#include "Engine.h"
#include "VDP.h"
#include "Timer.h"

namespace Tunnels {

namespace Utils { class StringSpan; }

namespace Classic {

class ScreenEngine : public Tunnels::ScreenEngine {
private:
  Tunnels::Timer::TimerManager &timerManager;
  VDP::Screen screen;
  VDP::Backend &backend;
  const Database *database;
  enum {
    APAC_CORRIDOR = 0,
    APAC_ROOM = 1,
    APAC_MENU = 2
  } activePatternsAndColors;
  byte fixturePattern[32];
  bool altCursor;
  unsigned vaultLine;
  class CursorTimer : public Timer::Timer {
  private:
    ScreenEngine &engine;
  protected:
    virtual void callback() override { engine.cursorCallback(getExpiry()); }
  public:
    CursorTimer(ScreenEngine &engine_) : engine(engine_) {}
  } cursorTimer;
  class BorderTimer : public Timer::Timer {
  private:
    ScreenEngine &engine;
  protected:
    virtual void callback() override { engine.borderCallback(getExpiry()); }
  public:
    BorderTimer(ScreenEngine &engine_) : engine(engine_) {}
  } borderTimer;

public:
  ScreenEngine(VDP::Backend &backend_, Tunnels::Timer::TimerManager &timerManager_) :
    timerManager(timerManager_), backend(backend_), altCursor(false),
    cursorTimer(*this), borderTimer(*this) { setDatabase(nullptr); }

  virtual void refresh() override;
  virtual void setDatabase(const Database *db) override;
  virtual void drawTitleScreen() override;
  virtual void initMenu() override;
  virtual void drawPrompt(unsigned n, unsigned param) override;
  virtual void setCursorEnabled(bool enabled) override;
  virtual void enableAlternateCursor() override;
  virtual void setBorderFlashEnabled(bool enabled) override;
  virtual void markSelection(byte ch) override;
  virtual void prepareStringInputField(unsigned len) override;
  virtual void endStringInputField(unsigned len, unsigned cnt) override;
  virtual void replaceStringInputField(Utils::StringSpan str) override;
  virtual void addStringInputField(byte ch) override;
  virtual void eraseStringInputField(unsigned cnt) override;
  virtual void drawIoError(bool casette, byte error) override;
  virtual void drawTradingScreen(bool itemProvided) override;
  virtual void preparePlayerNameInput(unsigned n) override;
  virtual void preparePlayerClassInput() override;
  virtual void preparePlayerColorInput() override;
  virtual void prepareItemNumberInput() override;
  virtual void prepareGiveItemReceiverInput() override;
  virtual void prepareTradingNumberInput() override;
  virtual void prepareVaultPlayerInput() override;
  virtual void prepareVaultDigitInput() override;
  virtual void updateVaultTable(unsigned cnt, int dir, unsigned correctDigits) override;
  virtual void drawCurrentPartyOrder() override;
  virtual void askCharacterAccept() override;
  virtual void setPlayerColors() override;
  virtual void setPlayerShapes(unsigned n) override;
  virtual void setRoomFixtureShapes() override;
  virtual void roomScreen() override;
  virtual void corridorScreen() override;
  virtual void mapScreen() override;
  virtual void clearMessages() override;
  virtual void clearVaultText() override;
  virtual void drawGameOver() override;
  virtual void drawVault() override;
  virtual void drawGeneralStore() override;
  virtual void drawDynamicFixture() override;
  virtual void drawLivingStatue() override;
  virtual void drawFountain() override;
  virtual void drawChestItem() override;
  virtual void drawMoneyItem() override;
  virtual void drawLootItem(unsigned index, ItemCategory cat, byte id) override;
  virtual void clearLootItem(unsigned index) override;
  virtual void drawLootItemName(ItemCategory cat, byte id) override;
  virtual void stairMovement(bool ascending) override;
  virtual void drawCorridorSegment(unsigned n, Location loc) override;
  virtual void drawCorridorLeftJunction(unsigned n, Location loc) override;
  virtual void drawCorridorRightJunction(unsigned n, Location loc) override;
  virtual void drawCorridorLeftWall(unsigned n) override;
  virtual void drawCorridorRightWall(unsigned n) override;
  virtual void drawDoorReveal(unsigned n) override;
  virtual void showCompass(Direction dir) override;
  virtual void drawPlayer(unsigned n) override;
  virtual bool isPlayerBlocked(unsigned n) override;
  virtual void drawMonster(unsigned n) override;
  virtual bool isMonsterBlocked(unsigned n) override;
  virtual void drawPlayerStatusHeader(unsigned n) override;
  virtual void drawWeaponChoice(ItemCategory cat, byte id, byte damage, byte ammo) override;
  virtual void drawArmorChoice(byte id, byte protection, bool shield) override;
  virtual void drawMagicItemChoice(byte id, byte uses) override;
  virtual void drawMagicEffectDescription(byte id) override;
  virtual void drawMagicItemDescription(unsigned id) override;
  virtual void draw1Choice() override;
  virtual void draw2Choice() override;
  virtual bool checkIfRoomSquareOccupied(unsigned y, unsigned x) override;
  void drawPrompt(unsigned n) { drawPrompt(n, 0); }

private:
  void menuScreen();
  void initRoom();
  void clearRoom();
  void initCorridor();
  void clearMapOrCorridor();
  void drawCorridorSideWall(unsigned x0, unsigned x1);
  void drawCorridorFloor(unsigned y0, unsigned y1);
  unsigned putDigit(unsigned y, unsigned x, unsigned d);
  unsigned putNumber(unsigned y, unsigned x, byte n);
  unsigned putNumber(unsigned y, unsigned x, int8 n);
  unsigned putNumber(unsigned y, unsigned x, uint16 n);
  unsigned putNumberEol(unsigned y, byte n);
  unsigned putNumberEol(unsigned y, uint16 n);
  unsigned putParenthesizedNumberEol(unsigned y, byte n);
  unsigned putPlural();
  bool isQuadBlocked(unsigned y, unsigned x);
  void putQuad(unsigned y, unsigned x, byte base);
  void putQuad(unsigned y, unsigned x, Utils::StringSpan tiles);
  void putWeaponDescription(unsigned y, unsigned x, ItemCategory cat,
			    byte id, byte damage, byte ammo, bool showDamage);
  void putWeaponDescription(unsigned y, unsigned x, unsigned player,
			    bool secondWeapon, bool showDamage);
  void putArmorDescription(unsigned y, unsigned x, unsigned player, bool shield);
  unsigned findEndOfLine();
  void promptExtension(byte n, unsigned param = 0);
  void cursorCallback(uint32 expiry);
  void borderCallback(uint32 expiry);
};

}
}

#endif // TUNNELS_CLASSIC_SCREENENGINE_H_
