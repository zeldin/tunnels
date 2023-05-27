#include "system.h"

#include "classic/SoundEngine.h"
#include "MusicBuilder.h"

namespace Tunnels { namespace Classic {

namespace {
using namespace DCSG::MusicBuilder;
static constexpr const auto titleMusic =
  music(INDEX<0>(),
	BEAT(20, 0xff, 0x8a, 0x2f, 0x93, 0xbf, 0xdf),
	JUMP<-1>(),
	INDEX<1>(),
	BEAT(20, 0xff, 0x8a, 0x2f, 0x93, 0xae, 0x0f, 0xb0, 0xdf),
	JUMP<-1>(),
	INDEX<-2>(),
	BEAT(20, 0x8a, 0x2f, 0xae, 0x0f),
	INDEX<-1>(),
	BEAT(10, 0x9f),
	BEAT(20, 0x93),
	BEAT(10, 0x87, 0x2a, 0xad, 0x11),
	BEAT(10, 0x81, 0x28, 0xa0, 0x14),
	BEAT(10, 0xa3, 0x15),
	BEAT(10, 0xad, 0x17),
	BEAT(20, 0x8a, 0x2f, 0xa4, 0x19),
	BEAT(10, 0xad, 0x17),
	BEAT(10, 0x8b, 0x23, 0xad, 0x11),
	BEAT(10, 0xad, 0x17),
	BEAT(10, 0xad, 0x11),
	BEAT(10, 0x8d, 0x1f, 0xa0, 0x0f),
	BEAT(10, 0xad, 0x11),
	BEAT(10, 0xa0, 0x0f),
	BEAT(10, 0x80, 0x1e, 0xae, 0x0b),
	BEAT(10, 0xa0, 0x0f),
	BEAT(10, 0xa4, 0x0b),
	BEAT(30, 0x8b, 0x23, 0xae, 0x0b),
	BEAT(20, 0x87, 0x2a, 0xa3, 0x0e),
	BEAT(10, 0x9f),
	BEAT(20, 0x93),
	BEAT(10, 0x8d, 0x25, 0xae, 0x0f),
	BEAT(10, 0x8b, 0x21, 0xad, 0x10),
	BEAT(10, 0xae, 0x12),
	BEAT(10, 0xa3, 0x15),
	BEAT(20, 0x87, 0x2a, 0xaa, 0x0a),
	BEAT(10, 0xa0, 0x0a),
	BEAT(30, 0x81, 0x28, 0xae, 0x0b),
	BEAT(30, 0x8d, 0x1f),
	BEAT(10, 0x8d, 0x17),
	BEAT(10, 0xaf, 0x07),
	BEAT(10, 0xaf, 0x08),
	BEAT(10, 0x81, 0x28, 0xa0, 0x0a),
	BEAT(10, 0xaa, 0x0a),
	BEAT(10, 0xae, 0x0b),
	BEAT(10, 0x8b, 0x23, 0xaa, 0x0a),
	BEAT(10, 0xaa, 0x0c),
	BEAT(10, 0xaa, 0x0a),
	BEAT(10, 0x80, 0x1e, 0xaf, 0x08),
	BEAT(10, 0xaa, 0x0a),
	BEAT(10, 0xaf, 0x08),
	BEAT(10, 0x84, 0x19, 0xa8, 0x07),
	BEAT(10, 0xaf, 0x08),
	BEAT(10, 0xa8, 0x07),
	BEAT(30, 0x83, 0x15, 0xa5, 0x06),
	BEAT(30, 0x8d, 0x1f),
	BEAT(30, 0x84, 0x19, 0xa5, 0x05),
	BEAT(30, 0x8d, 0x17, 0xa0, 0x05),
	BEAT(30, 0x8d, 0x1f),
	BEAT(10, 0x80, 0x1e, 0xa7, 0x04),
	BEAT(10, 0xaf, 0x08),
	BEAT(10, 0x9f, 0xaf, 0x07),
	BEAT(10, 0x93, 0xa8, 0x07),
	BEAT(10, 0xaf, 0x07),
	BEAT(10, 0x8c, 0x1a, 0xaf, 0x08),
	BEAT(10, 0x8d, 0x17, 0xa7, 0x04),
	BEAT(10, 0xaf, 0x08),
	BEAT(10, 0xaf, 0x07),
	BEAT(10, 0x80, 0x1e, 0xa8, 0x07),
	BEAT(10, 0xaf, 0x07),
	BEAT(10, 0xaf, 0x08),
	BEAT(10, 0x85, 0x1c, 0xa7, 0x08),
	BEAT(10, 0xa0, 0x0a),
	BEAT(10, 0xa7, 0x08),
	BEAT(10, 0x8d, 0x17, 0xa1, 0x07),
	BEAT(10, 0xa7, 0x08),
	BEAT(10, 0xa1, 0x07),
	BEAT(10, 0x80, 0x14, 0xaf, 0x05),
	BEAT(10, 0xa1, 0x07),
	BEAT(10, 0xaf, 0x05),
	BEAT(10, 0x8d, 0x10, 0xa0, 0x05),
	BEAT(10, 0xaf, 0x05),
	BEAT(10, 0x9f, 0xa0, 0x05),
	BEAT(10, 0x93, 0xaf, 0x05),
	BEAT(10, 0x8e, 0x0f, 0xa8, 0x07),
	BEAT(10, 0x9f, 0xaf, 0x07),
	BEAT(10, 0x8b, 0x21, 0x93, 0xaf, 0x08),
	BEAT(10, 0x8d, 0x1f, 0xa0, 0x0a),
	BEAT(10, 0x9f, 0xaa, 0x0a),
	BEAT(10, 0x89, 0x3f, 0x93, 0xae, 0x0f),
	BEAT(10, 0xa0, 0x0f),
	BEAT(10, 0x9f, 0xae, 0x0f),
	BEAT(10, 0x93, 0xad, 0x11),
	BEAT(10, 0x8a, 0x38, 0xa0, 0x14),
	BEAT(10, 0x87, 0x32, 0xa3, 0x15),
	JUMP<-2>());
}

void SoundEngine::playTitleMusic()
{
  titleMusic.play<1>(music);
  timerManager.setTimeoutAfter(7150);
}

void SoundEngine::playStairMusic()
{
  titleMusic.play<0>(music);
}

}}
