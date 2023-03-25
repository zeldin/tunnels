#include "system.h"

#include "Engine.h"

namespace Tunnels {

GameEngine::Checkpoint GameEngine::loadSave()
{
  redoTarget = CHECKPOINT_LOAD_SAVE;
  procdTarget = CHECKPOINT_NEW_OR_RESTOCK;
  acceptMask = ACCEPT_REDO | ACCEPT_NUMERIC;
  screen.drawPrompt(0x11);
  byte x;
  Checkpoint cp;
  if (progression != 2) {
    acceptMask |= ACCEPT_PROCD;
    screen.drawPrompt(0x13);
    cp = getNumberKey(x, 1, 6);
  } else
    cp = getNumberKey(x, 1, 3);
  if(cp)
    return cp;
  sound.beep();
  if (x > 2) {
    x -= 3;
  }
  switch (x) {
  case 0:
    screen.drawPrompt(0x18);
    screen.drawPrompt(0x0d);
    waitForEvent();
    return CHECKPOINT_QUIT;
    break;
  case 1:
    screen.drawPrompt(0x14);
    acceptMask = ACCEPT_REDO | ACCEPT_NUMERIC | ACCEPT_ALPHANUMERIC;
    byte filename[8];
    cp = getString(filename);
    break;
  case 2:
    screen.drawPrompt(0x15);
    acceptMask = ACCEPT_REDO | ACCEPT_ALPHANUMERIC2;
    byte devname[28];
    cp = getString(devname);
    break;
  }
  if (cp)
    return cp;
  waitForEvent();
  return CHECKPOINT_QUIT;
}

}
