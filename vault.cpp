#include "system.h"

#include "Engine.h"
#include "EventLoop.h"
#include "Database.h"

namespace Tunnels {

GameEngine::Diversion GameEngine::vault()
{
  screen.clearVaultText();
  roomDone = -1;
  byte maxDigit;
  unsigned numDigits;
  database->getRoomVaultParameters(currentRoom, maxDigit, numDigits);
  byte combination[numDigits];
  for (unsigned i=0; i<numDigits; i++)
    do
      combination[i] = random(8)+'1';
    while (combination[i] > maxDigit);
  unsigned inputPos, correctDigits;
  int penalty, dir;
  byte kc = KEY_REDO;
  for (;;) {
    if (kc == KEY_REDO) {
      acceptMask = ACCEPT_NUMERIC | ACCEPT_ALPHANUMERIC;
      screen.clearMessages();
      screen.drawPrompt(0x7e, currentRoom);
      screen.prepareVaultPlayerInput();
      if (Diversion d = getNamedPlayer())
	return d;
      int player = database->getCurrentPlayer();
      if (player < 0)
	return DIVERSION_CONTINUE_GAME;
      if (database->getPlayerWD(player) >= 
	  database->getPlayerHP(player)) {
	sound.honk();
	continue;
      }
      screen.clearMessages();
      screen.drawPrompt(0x7f);
      kc = KEY_ERASE;
    }
    if (kc == KEY_ERASE || kc == KEY_LEFT) {
      screen.prepareVaultDigitInput();
      inputPos = 0;
      dir = 0;
      screen.prepareStringInputField(numDigits);
      correctDigits = 0;
      penalty = 0;
    }
    random(255);
    screen.enableAlternateCursor();
    for (;;) {
      Event e = nextEvent();
      if (e.type() != EVENT_KEY) {
	if (e.type() == EVENT_QUIT) {
	  screen.setCursorEnabled(false);
	  return DIVERSION_QUIT;
	}
	continue;
      }
      kc = e.keycode();
      break;
    }
    screen.setCursorEnabled(false);
    if (kc == KEY_BACK)
      return DIVERSION_CONTINUE_GAME;
    if (kc < '1' || kc > maxDigit)
      continue;
    screen.addStringInputField(kc);
    sound.playVaultDigitSound(kc - '0');
    if (kc == combination[inputPos])
      correctDigits ++;
    else {
      --penalty;
      if (!dir)
	dir = (kc < combination[inputPos]? -1 : 1);
    }
    if (++inputPos < numDigits)
      continue;
    screen.endStringInputField(numDigits, inputPos);
    kc = KEY_ERASE;
    screen.updateVaultTable(numDigits, dir, correctDigits);
    if (correctDigits == numDigits) {
      screen.drawPrompt(0x80);
      sound.playVaultCombinationFoundMusic();
      if (Diversion d = flashBorder())
	return d;
      database->clearRoomVault(currentRoom);
      roomDone = 0;
      return DIVERSION_CONTINUE_GAME;
    }
    // FIXME: G@>C3DE
  }
}

}
