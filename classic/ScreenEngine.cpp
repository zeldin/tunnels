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

void ScreenEngine::refresh()
{
  screen.refresh(backend);
}

void ScreenEngine::setDatabase(const Database *db)
{
  database = db;
  activePatternTable = 2;
  if (!db)
    return;
  screen.loadPatterns(0, database->getPatternTable());
  if (!database->alternateHighPatternsActive())
    activePatternTable = 1;
}

}}
