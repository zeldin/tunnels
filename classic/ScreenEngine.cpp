#include "system.h"

#include "classic/ScreenEngine.h"
#include "Utils.h"
#include "Database.h"

namespace Tunnels { namespace Classic {

void ScreenEngine::markSelection(byte ch)
{
  do {
    if (screen.gchar(screen.getYpt(), screen.getXpt()) == ch) {
      screen.setXpt(screen.getXpt()-1);
      screen.hchar(screen.getYpt(), screen.getXpt(), 0x5c);
      return;
    }
    screen.setYpt(screen.getYpt()+1);
  } while(screen.getYpt() < 23);
}

void ScreenEngine::prepareStringInputField(unsigned len)
{
  if (len)
    screen.hchar(screen.getYpt(), screen.getXpt(), ']', len);
}

void ScreenEngine::endStringInputField(unsigned len, unsigned cnt)
{
  if (cnt < len)
    screen.hchar(screen.getYpt(), screen.getXpt(), ' ', len-cnt);
  screen.setXpt(screen.getXpt()-cnt);
}

void ScreenEngine::replaceStringInputField(Utils::StringSpan str)
{
  screen.hstr(screen.getYpt(), screen.getXpt(), str);
  screen.setXpt(screen.getXpt()+str.len());
}

void ScreenEngine::addStringInputField(byte ch)
{
  screen.hchar(screen.getYpt(), screen.getXpt(), ch);
  screen.setXpt(screen.getXpt()+1);
}

void ScreenEngine::eraseStringInputField(unsigned cnt)
{
  if (screen.gchar(screen.getYpt(), screen.getXpt()) == '^')
    screen.hchar(screen.getYpt(), screen.getXpt(), ']');
  if (cnt) {
    screen.setXpt(screen.getXpt()-cnt);
    screen.hchar(screen.getYpt(), screen.getXpt(), ']', cnt);
  }
}

void ScreenEngine::drawIoError(bool casette, byte error)
{
  screen.setYpt(19);
  screen.setXpt(3);
  if (casette) {
    screen.all(' ');
    screen.setYpt(11);
  }
  drawPrompt(0x6e);
  screen.hchar(screen.getYpt(), screen.getXpt(), byte('0'+error));
  screen.setYpt(screen.getYpt()+1);
  screen.setXpt(3);
}

void ScreenEngine::preparePlayerNameInput(unsigned n)
{
  screen.setYpt(3);
  screen.setXpt(findEndOfLine());
  screen.hchar(screen.getYpt(), screen.getXpt(),
	       byte(screen.gchar(screen.getYpt(), screen.getXpt()) + (n+1)));
  screen.setYpt(screen.getYpt() + 2);
  screen.setXpt(findEndOfLine() + 1);
}

void ScreenEngine::preparePlayerClassInput()
{
  screen.setYpt(7);
  screen.setXpt(21);
}

void ScreenEngine::preparePlayerColorInput()
{
  screen.setYpt(9);
  screen.setXpt(21);
}

void ScreenEngine::prepareItemNumberInput()
{
  screen.setYpt(17);
  screen.setXpt(findEndOfLine() + 2);
}

void ScreenEngine::prepareGiveItemReceiverInput()
{
  screen.setYpt(19);
  screen.setXpt(findEndOfLine() + 2);
}

void ScreenEngine::prepareVaultPlayerInput()
{
  screen.setYpt(20);
  screen.setXpt(findEndOfLine() + 2);
}

void ScreenEngine::prepareVaultDigitInput()
{
  int player = database->getCurrentPlayer();
  putNumber(20, 6, database->getPlayerHP(player));
  putNumber(20, 16, database->getPlayerWD(player));
  unsigned x = screen.getXpt(), y = screen.getYpt();
  while (x>0 && screen.gchar(y, x) != '?')
    --x;
  screen.setXpt(x+2);
}

void ScreenEngine::updateVaultTable(unsigned cnt, int dir, unsigned correctDigits)
{
  screen.hchar(vaultLine == 0? 17 : vaultLine - 1, 21, ' ');
  screen.hstr(vaultLine, 22, screen.gstr(screen.getYpt(), screen.getXpt(), cnt));
  screen.hchar(vaultLine, 21, '\\');
  screen.hchar(vaultLine, 28, (dir? (dir < 0? '<' : '>') : '+'));
  screen.hchar(vaultLine, 29, correctDigits + '0');
  if (++vaultLine > 17)
    vaultLine = 0;
}

void ScreenEngine::askCharacterAccept()
{
  screen.setYpt(11);
  screen.setXpt(2);
  drawPrompt(0x17);
}

void ScreenEngine::setPlayerColors()
{
  byte color_table[4];
  for (unsigned i=0; i<4; i++)
    color_table[i] = database->getPlayerColor(i);
  screen.loadColorTable(0, color_table);
}

void ScreenEngine::setPlayerShapes(unsigned n)
{
  screen.loadPatterns(n << 3,
		database->getClassPatternTable(database->getPlayerClass(n)));
}

void ScreenEngine::setRoomFixtureShapes()
{
  Utils::StringSpan patterns = database->getPatternTable();
  patterns.subspan(240*8, 12*8);
  screen.loadPatterns(240, patterns);
  patterns.subspan(4*8, 4*8);
  patterns.store(fixturePattern);
}

void ScreenEngine::refresh()
{
  screen.refresh(backend);
}

void ScreenEngine::setDatabase(const Database *db)
{
  database = db;
  activePatternsAndColors = APAC_MENU;
  if (!db)
    return;
  Utils::StringSpan patterns = database->getPatternTable();
  screen.loadPatterns(0, patterns);
  patterns.subspan(244*8, 4*8);
  patterns.store(fixturePattern);
  screen.loadSpritePatterns(0x80, database->getSpritePatternTable());
  if (!database->alternateHighPatternsActive())
    activePatternsAndColors = APAC_ROOM;
}

}}
