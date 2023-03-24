#ifndef TUNNELS_CLASSIC_SCREENENGINE_H_
#define TUNNELS_CLASSIC_SCREENENGINE_H_

#include "Engine.h"
#include "VDP.h"
#include "Timer.h"

namespace Tunnels {

namespace Classic {

class ScreenEngine : Timer::Timer, public Tunnels::ScreenEngine {
private:
  Tunnels::Timer::TimerManager &timerManager;
  VDP::Screen screen;
  VDP::Backend &backend;

public:
  ScreenEngine(VDP::Backend &backend_, Tunnels::Timer::TimerManager &timerManager_) :
    timerManager(timerManager_), backend(backend_) {}

  virtual void refresh() override;
  virtual void drawTitleScreen() override;
  virtual void initMenu() override;
  virtual void drawPrompt(unsigned n) override;
  virtual void setCursorEnabled(bool enabled) override;
  virtual void markSelection(byte ch) override;

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

protected:
  virtual void callback() override;
};

}
}

#endif // TUNNELS_CLASSIC_SCREENENGINE_H_
