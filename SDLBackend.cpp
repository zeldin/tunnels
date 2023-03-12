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
		       surface_->pitch},
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
	     SDL_CreateRGBSurface(SDL_SWSURFACE, VDP::PIXELS_W, VDP::PIXELS_H,
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

Event SDLBackend::handleEvent(const SDL_Event &event)
{
  if (event.type == SDL_QUIT)
    return Event::quitEvent();
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
				    VDP::PIXELS_W, VDP::PIXELS_H,
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
