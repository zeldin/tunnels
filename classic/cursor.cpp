#include "system.h"

#include "classic/ScreenEngine.h"

namespace Tunnels { namespace Classic {

void ScreenEngine::setCursorEnabled(bool enabled)
{
  if (enabled)
    timerManager.addTimer(*this);
  else
    timerManager.removeTimer(*this);
}

void ScreenEngine::callback()
{
  byte old = screen.gchar(screen.getYpt(), screen.getXpt());
  if (old == ']' || old == '^') {
    screen.hchar(screen.getYpt(), screen.getXpt(),
		 ((getExpiry()&256)? ']' : '^'));
    refresh();
  }
  timerManager.addTimer(*this, (getExpiry()|255)+1);
}

}}
