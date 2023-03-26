#ifndef TUNNELS_CLASSIC_SCREENENGINE_H_
#define TUNNELS_CLASSIC_SCREENENGINE_H_

#include "Engine.h"
#include "VDP.h"
#include "Timer.h"

namespace Tunnels {

namespace Classic {

class ScreenEngine : public Tunnels::ScreenEngine {
private:
  Tunnels::Timer::TimerManager &timerManager;
  VDP::Screen screen;
  VDP::Backend &backend;
  const Database *database;
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
    timerManager(timerManager_), backend(backend_), database(nullptr),
    cursorTimer(*this), borderTimer(*this) {}

  virtual void refresh() override;
  virtual void setDatabase(const Database *db) override;
  virtual void drawTitleScreen() override;
  virtual void initMenu() override;
  virtual void drawPrompt(unsigned n) override;
  virtual void setCursorEnabled(bool enabled) override;
  virtual void setBorderFlashEnabled(bool enabled) override;
  virtual void markSelection(byte ch) override;
  virtual void prepareStringInputField(unsigned len) override;
  virtual void endStringInputField(unsigned len, unsigned cnt) override;
  virtual void addStringInputField(byte ch) override;
  virtual void eraseStringInputField(unsigned cnt) override;
  virtual void drawIoError(bool casette, byte error) override;
  virtual void setPlayerColors() override;

private:
  void menuScreen();
  unsigned putDigit(unsigned y, unsigned x, unsigned d);
  unsigned putNumber(unsigned y, unsigned x, byte n);
  unsigned putNumber(unsigned y, unsigned x, uint16 n);
  unsigned putNumberEol(unsigned y, byte n);
  unsigned putNumberEol(unsigned y, uint16 n);
  unsigned putPlural();
  void putQuad(unsigned y, unsigned x, byte base);
  unsigned findEndOfLine();
  void promptExtension(byte n);
  void cursorCallback(uint32 expiry);
  void borderCallback(uint32 expiry);
};

}
}

#endif // TUNNELS_CLASSIC_SCREENENGINE_H_
