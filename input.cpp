#include "system.h"

#include "Engine.h"
#include "EventLoop.h"

namespace Tunnels {

GameEngine::Diversion GameEngine::rawGetKey(byte &kc, bool nonBlock)
{
  for (;;) {
    Event e = nextEvent(nonBlock);
    if (nonBlock && !e) {
      kc = 0;
      return DIVERSION_NULL;
    }
    if (e.type() != EVENT_KEY) {
      if (e.type() == EVENT_QUIT)
	return DIVERSION_QUIT;
      continue;
    }
    kc = e.keycode();
    return DIVERSION_NULL;
  }
}

GameEngine::Diversion GameEngine::getKeyNoCursor(byte &kc)
{
  for (;;) {
    if (Diversion d = rawGetKey(kc))
      return d;
    if ((kc >= '0' && kc <= '9') ||
	kc == KEY_ERASE || kc == KEY_LEFT || kc == KEY_ENTER) {
      if (!(acceptMask &
	    (ACCEPT_NUMERIC | ACCEPT_ALPHANUMERIC | ACCEPT_ALPHANUMERIC2)))
	continue;
      /* @>8356 = >36DB */
      return DIVERSION_NULL;
    } else if (kc < ' ')
      switch(kc) {
      case KEY_AID:
	if ((acceptMask & ACCEPT_AID))
	  return DIVERSION_NULL;
	break;
      case KEY_REDO:
	if ((acceptMask & ACCEPT_REDO))
	  return redoTarget;
	break;
      case KEY_DOWN:
      case KEY_UP:
	if ((acceptMask & ACCEPT_UP_DOWN))
	  return DIVERSION_NULL;	
	break;
      case KEY_PROCD:
	if ((acceptMask & ACCEPT_PROCD))
	  return procdTarget;
	break;
      case KEY_BEGIN:
	if (progression >= 4) {
	  screen.initMenu();
	  return DIVERSION_NEW_OR_RESTOCK;
	}
	break;
      case KEY_BACK:
	if ((acceptMask & ACCEPT_BACK))
	  return backTarget;
	break;
      }
    else {
      if ((acceptMask & ACCEPT_ALPHANUMERIC))
	return DIVERSION_NULL;
      if ((acceptMask & ACCEPT_ALPHANUMERIC2)) {
	/* @>8356 = >36DB */
	return DIVERSION_NULL;
      }
    }
    sound.honk();
  }
}

GameEngine::Diversion GameEngine::getKey(byte &kc)
{
  screen.setCursorEnabled(true);
  Diversion d = getKeyNoCursor(kc);
  screen.setCursorEnabled(false);
  return d;
}

GameEngine::Diversion GameEngine::getNumberKey(byte &n, byte low, byte high)
{
  for (;;) {
    byte kc;
    Diversion d = getKey(kc);
    if (d)
      return d;
    byte v = kc - '0';
    if (v >= low && v <= high) {
      screen.markSelection(kc);
      n = v-1;
      sound.beep();
      return DIVERSION_NULL;
    }
    sound.honk();
  }
}

GameEngine::Diversion GameEngine::getString(unsigned len, byte *result)
{
  screen.prepareStringInputField(len);
  unsigned cnt = 0;
  for (;;) {
    byte kc;
    Diversion d = getKey(kc);
    if (d)
      return d;
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
	  return DIVERSION_AID;
	case KEY_UP:
	  return DIVERSION_UP;
	case KEY_DOWN:
	  return DIVERSION_DOWN;
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
  return DIVERSION_NULL;
}

GameEngine::Diversion GameEngine::getNumber(unsigned lower, unsigned upper,
					    unsigned &value, bool x10)
{
  unsigned numDigs = x10, limit = 1;
  while (upper >= limit) {
    numDigs ++;
    limit *= 10;
  }
  byte digits[numDigs];
  for (;;) {
    Diversion d = getString(numDigs, digits);
    if (d)
      return d;
    unsigned n = 0;
    for(unsigned i=0; i<numDigs; i++)
      if (digits[i] >= '0' && digits[i] <= '9')
	n = n*10 + (digits[i]-'0');
      else
	break;
    if (x10)
      n /= 10;
    if (n >= lower && n <=upper) {
      value = n;
      return DIVERSION_NULL;
    }
    sound.honk();
  }
}

}
