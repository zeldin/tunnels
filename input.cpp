#include "system.h"

#include "Engine.h"
#include "EventLoop.h"

namespace Tunnels {

GameEngine::Checkpoint GameEngine::getKey(byte &kc)
{
  Checkpoint cp = CHECKPOINT_NULL;
  screen.setCursorEnabled(true);
  for (;;) {
    Event e = nextEvent();
    if (e.type() != EVENT_KEY) {
      if (e.type() == EVENT_QUIT) {
	cp = CHECKPOINT_QUIT;
	break;
      }
      continue;
    }
    kc = e.keycode();
    if (kc == KEY_REDO && (acceptMask & ACCEPT_REDO)) {
      cp = redoTarget;
      break;
    }
    if (kc == KEY_PROCD && (acceptMask & ACCEPT_PROCD)) {
      cp = procdTarget;
      break;
    }
    if (kc == KEY_BACK && (acceptMask & ACCEPT_BACK)) {
      cp = backTarget;
      break;
    }
    if (kc == KEY_BEGIN && progression >= 4) {
      cp = CHECKPOINT_NEW_OR_RESTOCK;
      break;
    }
    if (kc >= '1' && kc <= '9')
      break;
    sound.honk();
  }
  screen.setCursorEnabled(false);
  return cp;
}

GameEngine::Checkpoint GameEngine::getNumberKey(byte &n, byte low, byte high)
{
  for (;;) {
    byte kc;
    Checkpoint cp = getKey(kc);
    if (cp)
      return cp;
    byte v = kc - '0';
    if (v >= low && v <= high) {
      screen.markSelection(kc);
      n = v-1;
      sound.beep();
      return CHECKPOINT_NULL;
    }
    sound.honk();
  }
}

}
