#include "system.h"

#include "Engine.h"
#include "EventLoop.h"

namespace Tunnels {

GameEngine::Checkpoint GameEngine::getKeyNoCursor(byte &kc)
{
  for (;;) {
    Event e = nextEvent();
    if (e.type() != EVENT_KEY) {
      if (e.type() == EVENT_QUIT)
	return CHECKPOINT_QUIT;
      continue;
    }
    kc = e.keycode();
    if ((kc >= '0' && kc <= '9') ||
	kc == KEY_ERASE || kc == KEY_LEFT || kc == KEY_ENTER) {
      if (!(acceptMask &
	    (ACCEPT_NUMERIC | ACCEPT_ALPHANUMERIC | ACCEPT_ALPHANUMERIC2)))
	continue;
      /* @>8356 = >36DB */
      return CHECKPOINT_NULL;
    } else if (kc < ' ')
      switch(kc) {
      case KEY_AID:
	if ((acceptMask & ACCEPT_AID))
	  return CHECKPOINT_NULL;
	break;
      case KEY_REDO:
	if ((acceptMask & ACCEPT_REDO))
	  return redoTarget;
	break;
      case KEY_DOWN:
      case KEY_UP:
	if ((acceptMask & ACCEPT_UP_DOWN))
	  return CHECKPOINT_NULL;	
	break;
      case KEY_PROCD:
	if ((acceptMask & ACCEPT_PROCD))
	  return procdTarget;
	break;
      case KEY_BEGIN:
	if (progression >= 4) {
	  screen.initMenu();
	  return CHECKPOINT_NEW_OR_RESTOCK;
	}
	break;
      case KEY_BACK:
	if ((acceptMask & ACCEPT_BACK))
	  return backTarget;
	break;
      }
    else {
      if ((acceptMask & ACCEPT_ALPHANUMERIC))
	return CHECKPOINT_NULL;
      if ((acceptMask & ACCEPT_ALPHANUMERIC2)) {
	/* @>8356 = >36DB */
	return CHECKPOINT_NULL;
      }
    }
    sound.honk();
  }
}

GameEngine::Checkpoint GameEngine::getKey(byte &kc)
{
  screen.setCursorEnabled(true);
  Checkpoint cp = getKeyNoCursor(kc);
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

GameEngine::Checkpoint GameEngine::getString(unsigned len, byte *result)
{
  screen.prepareStringInputField(len);
  unsigned cnt = 0;
  for (;;) {
    byte kc;
    Checkpoint cp = getKey(kc);
    if (cp)
      return cp;
    if (kc == KEY_ENTER)
      break;
    else if (kc == KEY_ERASE) {
      screen.eraseStringInputField(cnt);
      cnt = 0;
      continue;
    } else if (kc == KEY_LEFT) {
      if (cnt > 0) {
	screen.eraseStringInputField(1);
	--cnt;
	continue;
      }
    } else if (cnt < len) {
      if (kc < 0x10) {
	switch (kc) {
	case KEY_AID:
	  return CHECKPOINT_AID;
	case KEY_UP:
	  return CHECKPOINT_UP;
	case KEY_DOWN:
	  return CHECKPOINT_DOWN;
	}
      } else {
	if (result)
	  result[cnt] = kc;
	cnt++;
	screen.addStringInputField(kc);
	continue;
      }
    }
    sound.honk();
  }
  screen.endStringInputField(len, cnt);
  if (result != nullptr)
    while(cnt < len)
      result[cnt++] = ' ';
  return CHECKPOINT_NULL;
}

}
