#include "system.h"

#include "classic/ScreenEngine.h"

namespace Tunnels { namespace Classic {

void ScreenEngine::refresh()
{
  screen.refresh(backend);
}

}}
