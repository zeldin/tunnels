#include "system.h"

#include "Engine.h"

namespace Tunnels {

GameEngine::Diversion GameEngine::loadSave()
{
  redoTarget = DIVERSION_LOAD_SAVE;
  procdTarget = DIVERSION_NEW_OR_RESTOCK;
  acceptMask = ACCEPT_REDO | ACCEPT_NUMERIC;
  screen.drawPrompt(0x11);
  byte x;
  Diversion d;
  if (progression != 2) {
    acceptMask |= ACCEPT_PROCD;
    screen.drawPrompt(0x13);
    d = getNumberKey(x, 1, 6);
  } else
    d = getNumberKey(x, 1, 3);
  if(d)
    return d;
  sound.beep();
  if (x > 2) {
    x -= 3;
  }
  switch (x) {
  case 0:
    screen.drawPrompt(0x18);
    screen.drawPrompt(0x0d);
    waitForEvent();
    return DIVERSION_QUIT;
    break;
  case 1:
    screen.drawPrompt(0x14);
    acceptMask = ACCEPT_REDO | ACCEPT_NUMERIC | ACCEPT_ALPHANUMERIC;
    byte filename[8];
    d = getString(filename);
    break;
  case 2:
    screen.drawPrompt(0x15);
    acceptMask = ACCEPT_REDO | ACCEPT_ALPHANUMERIC2;
    byte devname[28];
    d = getString(devicename);
    break;
  }
  if (d)
    return d;
  waitForEvent();
  return DIVERSION_QUIT;
}

}
