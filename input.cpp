#include "system.h"

#include "Engine.h"
#include "EventLoop.h"

namespace Tunnels {

Event GameEngine::getKey()
{
  Event e = Event::nullEvent();
  screen.setCursorEnabled(true);
  for (;;) {
    e = nextEvent();
    if (e.type() != EVENT_KEY)
      break;
    byte kc = e.keycode();
    if (kc >= '1' && kc <= '9')
      break;
    sound.honk();
  }
  screen.setCursorEnabled(false);
  return e;
}

Event GameEngine::getNumberKey(byte &n, byte low, byte high)
{
  for (;;) {
    Event e = getKey();
    if (e.type() != EVENT_KEY)
      return e;
    byte v = e.keycode() - '0';
    if (v >= low && v <= high) {
      screen.markSelection(e.keycode());
      n = v-1;
      sound.beep();
      return e;
    }
    sound.honk();
  }
}

}
