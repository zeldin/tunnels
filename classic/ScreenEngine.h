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
};

}
}

#endif // TUNNELS_CLASSIC_SCREENENGINE_H_