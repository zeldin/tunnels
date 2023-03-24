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
  waitForEvent();
  return CHECKPOINT_QUIT;
}

}
