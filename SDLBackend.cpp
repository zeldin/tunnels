#include "system.h"

#include "SDLBackend.h"

namespace Tunnels {

namespace {
struct SDLColor : SDL_Color {
  constexpr SDLColor(VDP::RGB rgb) : SDL_Color{.r = rgb.r, .g = rgb.g,
					       .b = rgb.b, .a = 0xff} { }
};
constexpr VDP::Palette<SDLColor> sdl_colors{VDP::palette};
}

SDLBackend::SDLBackend(SDL_Surface *screen_, SDL_Surface *surface_,
		       int sampleRate_) :
  VDP::BytemapRenderer{reinterpret_cast<byte *>(surface_->pixels),
		       surface_->pitch, BORDER_W, BORDER_H},
  DCSG::PCMSynthesizer(sampleRate_),
  screen{screen_},
  surface{surface_}
{
  SDL_SetPaletteColors(surface->format->palette,
		       &sdl_colors[0], 0, VDP::COLORS);

  SDL_AudioSpec desired = {
    .freq = sampleRate_,
    .format = AUDIO_S16SYS,
    .channels = 1,
    .samples = 512,
    .callback = fillAudio,
    .userdata = this
  };
  audioDevice = SDL_OpenAudioDevice(NULL, 0, &desired, &audioSpec, 0);
  if (audioDevice > 0)
    SDL_PauseAudioDevice(audioDevice, 0);
}

SDLBackend::SDLBackend(SDL_Surface *screen_, int sampleRate_) :
  SDLBackend{screen_,
	     SDL_CreateRGBSurface(SDL_SWSURFACE,
				  VDP::PIXELS_W + 2*BORDER_W,
				  VDP::PIXELS_H + 2*BORDER_H,
				  8, 0, 0, 0, 0), sampleRate_}
{
}

SDLBackend::~SDLBackend()
{
}

VDP::ScopedRender SDLBackend::startRender()
{
  return VDP::ScopedRender{this};
}

void SDLBackend::endRender()
{
  SDL_BlitSurface(surface, NULL, screen, NULL);
}

DCSG::ScopedSynthesis SDLBackend::startSynthesis()
{
  SDL_LockAudioDevice(audioDevice);
  return DCSG::ScopedSynthesis{this};
}

void SDLBackend::endSynthesis()
{
  SDL_UnlockAudioDevice(audioDevice);
}

byte SDLBackend::decodeKey(const SDL_Keysym &keysym)
{
  static const byte alphaFctn[26] = {
    '|', 0xbe, '`', 0x09, 0x0b,'{', '}', 0xbf, '?', 0xc0, 0xc1, 0xc2, 0xc3,
    0xc4, '\'', '"', 0xb9, '[', 0x08, ']', '_', 0x7f, '~', 0x0a, 0xc6, '\\'
  };
  static const byte numShift[15] = {
    ')', '!', '@', '#', '$', '%', '^', '&', '*', '(', '<', '>', '-', ':', '+'
  };
  static const byte numFctn[15] = {
    0xbc, 0x03, 0x04, 0x07, 0x02, 0x0e, 0x0c, 0x01, 0x06, 0x0f,
    0xb8, 0xb9, 0xba, 0xbd, 0x05
  };
  static const byte numCtrl[7] = {
    0x9e, 0x9f, 0x80, 0x9b, 0xbb, 0x9c, 0x9d
  };
  static const byte punct[5] = {
    ',', '.', '/', ';', '='
  };
  byte index;
  switch(keysym.scancode) {
  case SDL_SCANCODE_RETURN:
  case SDL_SCANCODE_KP_ENTER:
    return byte{KEY_ENTER};
  case SDL_SCANCODE_SPACE:
    return byte{' '};
  case SDL_SCANCODE_LEFT:
  case SDL_SCANCODE_KP_4:
    return byte{KEY_LEFT};
  case SDL_SCANCODE_RIGHT:
  case SDL_SCANCODE_KP_6:
    return byte{KEY_RIGHT};
  case SDL_SCANCODE_DOWN:
  case SDL_SCANCODE_KP_2:
    return byte{KEY_DOWN};
  case SDL_SCANCODE_UP:
  case SDL_SCANCODE_KP_8:
    return byte{KEY_UP};
  case SDL_SCANCODE_COMMA:
    index = 10;
    break;
  case SDL_SCANCODE_PERIOD:
    index = 11;
    break;
  case SDL_SCANCODE_SLASH:
    index = 12;
    break;
  case SDL_SCANCODE_SEMICOLON:
    index = 13;
    break;
  case SDL_SCANCODE_EQUALS:
    index = 14;
    break;
  case SDL_SCANCODE_F12:
    /* F12 -> FCTN = */
    return byte{KEY_QUIT};
  default:
    if (keysym.sym >= SDLK_0 && keysym.sym <= SDLK_9) {
      index = keysym.sym - SDLK_0;
      break;
    } else if (keysym.sym >= SDLK_a && keysym.sym <= SDLK_z) {
      /* Alphabetic key */
      index = keysym.sym - SDLK_a;
      if ((keysym.mod & KMOD_CTRL))
	return byte{0x81}+index;
      else if ((keysym.mod & (KMOD_ALT | KMOD_GUI)))
	return alphaFctn[index];
      else if ((keysym.mod & (KMOD_SHIFT | KMOD_CAPS)))
	return byte{'A'}+index;
      else
	return byte{'a'}+index;
    } else if (keysym.sym >= SDLK_SPACE &&
	       keysym.sym <= SDLK_UNDERSCORE) {
      /* Untranslated ASCII key */
      return static_cast<byte>(keysym.sym);
    } else if (keysym.sym >= SDLK_F1 && keysym.sym <= SDLK_F9) {
      /* F1-F9 -> FCTN 1-9 */
      return numFctn[(keysym.sym - SDLK_F1) + 1];
    }
    /* Unhandled key */
    return byte{0xff};
  }
  /* Number or punctuation key */
  if ((keysym.mod & KMOD_CTRL)) {
    if (index < 8)
      return byte{0xb0}+index;
    else
      return numCtrl[index-8];
  } else if ((keysym.mod & (KMOD_ALT | KMOD_GUI)))
    return numFctn[index];
  else if ((keysym.mod & KMOD_SHIFT))
    return numShift[index];
  else {
    if (index < 10)
      return byte{'0'}+index;
    else
      return punct[index-10];
  }
}

Event SDLBackend::handleEvent(const SDL_Event &event)
{
  if (event.type == SDL_QUIT)
    return Event::quitEvent();
  else if (event.type == SDL_KEYDOWN) {
    byte keyCode = decodeKey(event.key.keysym);

    /* Mode 3 */
    if (keyCode >= 'a' && keyCode <= 'z')
      keyCode -= 0x20;
    if (keyCode > 0x5f || (keyCode >= 0x10 && keyCode <= 0x1f))
      keyCode = 0xff;

    if (keyCode != 0xff)
      return Event::keyEvent(keyCode);
  }
  return Event::nullEvent();
}

Event SDLBackend::runEventLoop(Timer::TimerManager& timerManager)
{
  SDL_Event event;
  while (true) {
    uint32 now = SDL_GetTicks();
    uint32 delay = timerManager.getNextDelay(now);
    if (timerManager.timeoutExpired())
      return Event::timeoutEvent();
    else if (delay? SDL_WaitEventTimeout(&event, delay) :
	     SDL_WaitEvent(&event)) {
      Event handledEvent = handleEvent(event);
      if (handledEvent)
	return handledEvent;
    }
  }
}

SDLWindowBackend::SDLWindowBackend(SDL_Window *window_, int sampleRate) :
  SDLBackend{SDL_GetWindowSurface(window_), sampleRate}, window{window_}
{
}
    
SDLWindowBackend::SDLWindowBackend(const char *window_title, int sampleRate):
  SDLWindowBackend{SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED,
				    SDL_WINDOWPOS_CENTERED,
				    VDP::PIXELS_W + 2*BORDER_W,
				    VDP::PIXELS_H + 2*BORDER_H,
				    0), sampleRate}
{
}

SDLWindowBackend::~SDLWindowBackend()
{
  SDL_DestroyWindow(window);
}
  
void SDLWindowBackend::endRender()
{
  SDLBackend::endRender();
  SDL_UpdateWindowSurface(window);
}

Event SDLWindowBackend::handleEvent(const SDL_Event &event)
{
  if (event.type == SDL_WINDOWEVENT &&
      event.window.event == SDL_WINDOWEVENT_EXPOSED) {
    SDL_UpdateWindowSurface(window);
  }
  return SDLBackend::handleEvent(event);
}

}
