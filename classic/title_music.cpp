#include "system.h"

#include "classic/SoundEngine.h"

namespace Tunnels { namespace Classic {

void SoundEngine::playTitleMusic()
{
  static constexpr byte titleMusic[] {
    0x08, 0xff, 0x8a, 0x2f, 0x93, 0xae, 0x0f, 0xb0,
    0xdf, 0x14, 0x00, 0x00, 0x13, 0x04, 0x8a, 0x2f,
    0xae, 0x0f, 0x14, 0x01, 0x9f, 0x0a, 0x01, 0x93,
    0x14, 0x04, 0x87, 0x2a, 0xad, 0x11, 0x0a, 0x04,
    0x81, 0x28, 0xa0, 0x14, 0x0a, 0x02, 0xa3, 0x15,
    0x0a, 0x02, 0xad, 0x17, 0x0a, 0x04, 0x8a, 0x2f,
    0xa4, 0x19, 0x14, 0x02, 0xad, 0x17, 0x0a, 0x04,
    0x8b, 0x23, 0xad, 0x11, 0x0a, 0x02, 0xad, 0x17,
    0x0a, 0x02, 0xad, 0x11, 0x0a, 0x04, 0x8d, 0x1f,
    0xa0, 0x0f, 0x0a, 0x02, 0xad, 0x11, 0x0a, 0x02,
    0xa0, 0x0f, 0x0a, 0x04, 0x80, 0x1e, 0xae, 0x0b,
    0x0a, 0x02, 0xa0, 0x0f, 0x0a, 0x02, 0xa4, 0x0b,
    0x0a, 0x04, 0x8b, 0x23, 0xae, 0x0b, 0x1e, 0x04,
    0x87, 0x2a, 0xa3, 0x0e, 0x14, 0x01, 0x9f, 0x0a,
    0x01, 0x93, 0x14, 0x04, 0x8d, 0x25, 0xae, 0x0f,
    0x0a, 0x04, 0x8b, 0x21, 0xad, 0x10, 0x0a, 0x02,
    0xae, 0x12, 0x0a, 0x02, 0xa3, 0x15, 0x0a, 0x04,
    0x87, 0x2a, 0xaa, 0x0a, 0x14, 0x02, 0xa0, 0x0a,
    0x0a, 0x04, 0x81, 0x28, 0xae, 0x0b, 0x1e, 0x02,
    0x8d, 0x1f, 0x1e, 0x02, 0x8d, 0x17, 0x0a, 0x02,
    0xaf, 0x07, 0x0a, 0x02, 0xaf, 0x08, 0x0a, 0x04,
    0x81, 0x28, 0xa0, 0x0a, 0x0a, 0x02, 0xaa, 0x0a,
    0x0a, 0x02, 0xae, 0x0b, 0x0a, 0x04, 0x8b, 0x23,
    0xaa, 0x0a, 0x0a, 0x02, 0xaa, 0x0c, 0x0a, 0x02,
    0xaa, 0x0a, 0x0a, 0x04, 0x80, 0x1e, 0xaf, 0x08,
    0x0a, 0x02, 0xaa, 0x0a, 0x0a, 0x02, 0xaf, 0x08,
    0x0a, 0x04, 0x84, 0x19, 0xa8, 0x07, 0x0a, 0x02,
    0xaf, 0x08, 0x0a, 0x02, 0xa8, 0x07, 0x0a, 0x04,
    0x83, 0x15, 0xa5, 0x06, 0x1e, 0x02, 0x8d, 0x1f,
    0x1e, 0x04, 0x84, 0x19, 0xa5, 0x05, 0x1e, 0x04,
    0x8d, 0x17, 0xa0, 0x05, 0x1e, 0x02, 0x8d, 0x1f,
    0x1e, 0x04, 0x80, 0x1e, 0xa7, 0x04, 0x0a, 0x02,
    0xaf, 0x08, 0x0a, 0x03, 0x9f, 0xaf, 0x07, 0x0a,
    0x03, 0x93, 0xa8, 0x07, 0x0a, 0x02, 0xaf, 0x07,
    0x0a, 0x04, 0x8c, 0x1a, 0xaf, 0x08, 0x0a, 0x04,
    0x8d, 0x17, 0xa7, 0x04, 0x0a, 0x02, 0xaf, 0x08,
    0x0a, 0x02, 0xaf, 0x07, 0x0a, 0x04, 0x80, 0x1e,
    0xa8, 0x07, 0x0a, 0x02, 0xaf, 0x07, 0x0a, 0x02,
    0xaf, 0x08, 0x0a, 0x04, 0x85, 0x1c, 0xa7, 0x08,
    0x0a, 0x02, 0xa0, 0x0a, 0x0a, 0x02, 0xa7, 0x08,
    0x0a, 0x04, 0x8d, 0x17, 0xa1, 0x07, 0x0a, 0x02,
    0xa7, 0x08, 0x0a, 0x02, 0xa1, 0x07, 0x0a, 0x04,
    0x80, 0x14, 0xaf, 0x05, 0x0a, 0x02, 0xa1, 0x07,
    0x0a, 0x02, 0xaf, 0x05, 0x0a, 0x04, 0x8d, 0x10,
    0xa0, 0x05, 0x0a, 0x02, 0xaf, 0x05, 0x0a, 0x03,
    0x9f, 0xa0, 0x05, 0x0a, 0x03, 0x93, 0xaf, 0x05,
    0x0a, 0x04, 0x8e, 0x0f, 0xa8, 0x07, 0x0a, 0x03,
    0x9f, 0xaf, 0x07, 0x0a, 0x05, 0x8b, 0x21, 0x93,
    0xaf, 0x08, 0x0a, 0x04, 0x8d, 0x1f, 0xa0, 0x0a,
    0x0a, 0x03, 0x9f, 0xaa, 0x0a, 0x0a, 0x05, 0x89,
    0x3f, 0x93, 0xae, 0x0f, 0x0a, 0x02, 0xa0, 0x0f,
    0x0a, 0x03, 0x9f, 0xae, 0x0f, 0x0a, 0x03, 0x93,
    0xad, 0x11, 0x0a, 0x04, 0x8a, 0x38, 0xa0, 0x14,
    0x0a, 0x04, 0x87, 0x32, 0xa3, 0x15, 0x0a, 0x00,
    0x00, 0x0d
  };
  music.play(titleMusic);
  timerManager.setTimeoutAfter(7150);
}

}}
