#include "system.h"

#include "classic/ScreenEngine.h"
#include "Database.h"

namespace Tunnels { namespace Classic {

void ScreenEngine::setCursorEnabled(bool enabled)
{
  if (enabled)
    timerManager.addTimer(cursorTimer);
  else
    timerManager.removeTimer(cursorTimer);
}

void ScreenEngine::cursorCallback(uint32 expiry)
{
  byte old = screen.gchar(screen.getYpt(), screen.getXpt());
  if (old == ']' || old == '^') {
    screen.hchar(screen.getYpt(), screen.getXpt(),
		 ((expiry&256)? ']' : '^'));
    refresh();
  }
  timerManager.addTimer(cursorTimer, (expiry|255)+1);
}

void ScreenEngine::setBorderFlashEnabled(bool enabled)
{
  if (enabled) {
    timerManager.addTimer(borderTimer);
  } else {
    timerManager.removeTimer(borderTimer);
    if (activePatternsAndColors == APAC_MENU)
      screen.setBackground(VDP::DARK_RED);
    else
      screen.setBackground(VDP::LIGHT_GREEN);
  }
}

void ScreenEngine::borderCallback(uint32 expiry)
{
  if (expiry&1024)
    screen.setBackground(VDP::DARK_GREEN);
  else
    screen.setBackground(VDP::MEDIUM_GREEN);
  refresh();
  timerManager.addTimer(borderTimer, (expiry|1023)+1);
}

}}
