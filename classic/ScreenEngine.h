#ifndef TUNNELS_CLASSIC_SCREENENGINE_H_
#define TUNNELS_CLASSIC_SCREENENGINE_H_

#include "Engine.h"
#include "VDP.h"

namespace Tunnels {

namespace Classic {

class ScreenEngine : public Tunnels::ScreenEngine {
private:
  VDP::Screen screen;
  VDP::Backend &backend;

public:
  ScreenEngine(VDP::Backend &backend_) : backend(backend_) {}

  virtual void refresh() override;
  virtual void drawTitleScreen() override;
  virtual void drawPrompt(unsigned n) override;

private:
  void initScreen();
  unsigned putDigit(unsigned y, unsigned x, unsigned d);
  unsigned putNumber(unsigned y, unsigned x, byte d);
  unsigned putNumber(unsigned y, unsigned x, uint16 d);
  unsigned putPlural();
  void putQuad(unsigned y, unsigned x, byte base);
  unsigned findEndOfLine();
  void gplExtension(uint16 addr);
};

}
}

#endif // TUNNELS_CLASSIC_SCREENENGINE_H_
