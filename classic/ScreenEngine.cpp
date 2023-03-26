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

void ScreenEngine::setPlayerColors()
{
  byte color_table[4];
  for (unsigned i=0; i<4; i++)
    color_table[i] = database->getPlayerColor(i);
  screen.loadColorTable(0, color_table);
}

void ScreenEngine::refresh()
{
  screen.refresh(backend);
}

void ScreenEngine::setDatabase(const Database *db)
{
  database = db;
  if (!db)
    return;
  screen.loadPatterns(0, database->getPatternTable());
}

}}
