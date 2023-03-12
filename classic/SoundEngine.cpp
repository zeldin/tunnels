#include "system.h"

#include "classic/SoundEngine.h"

namespace Tunnels { namespace Classic {

void SoundEngine::stopMusic()
{
  static constexpr byte clearVoices[] {
    0x04, 0xff, 0xdf, 0xbf, 0x9f, 0x01, 0x01, 0xff, 0x00
  };
  music.play(clearVoices);
}

}}
