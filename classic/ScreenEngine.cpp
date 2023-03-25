#include "system.h"

#include "classic/ScreenEngine.h"

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

void ScreenEngine::refresh()
{
  screen.refresh(backend);
}

}}
