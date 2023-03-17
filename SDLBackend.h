#ifndef TUNNELS_SDLBACKEND_H_
#define TUNNELS_SDLBACKEND_H_

#include <SDL2/SDL.h>

#include "VDP.h"
#include "DCSG.h"
#include "EventLoop.h"

namespace Tunnels {

namespace Timer { class TimerManager; }

class SDLBackend : VDP::BytemapRenderer, DCSG::PCMSynthesizer,
		   public VDP::Backend, public DCSG::Backend,
		   public EventLoop {
private:
  SDL_Surface* screen;
  SDL_Surface* surface;
  SDL_AudioDeviceID audioDevice;
  SDL_AudioSpec audioSpec;
  static void fillAudio(void *udata, Uint8 *stream, int len) {
    reinterpret_cast<SDLBackend*>(udata)
      ->fillAudio(reinterpret_cast<int16*>(stream), len>>1);
  }

public:
  SDLBackend(SDL_Surface *screen_, int sampleRate = 44100);
  virtual ~SDLBackend() override;
  virtual VDP::ScopedRender startRender() override;
  virtual DCSG::ScopedSynthesis startSynthesis() override;
  virtual Event runEventLoop(Timer::TimerManager& timerManager) override;

protected:
  static constexpr unsigned BORDER_W = 8;
  static constexpr unsigned BORDER_H = 8;

  SDLBackend(SDL_Surface *screen_, SDL_Surface *surface_,
	     int sampleRate_);
  void fillAudio(int16 *buffer, int len) {
    PCMSynthesizer::generatePCM(buffer, len);
  }
  virtual void endRender() override;
  virtual void endSynthesis() override;
  virtual Event handleEvent(const SDL_Event &event);
  static byte decodeKey(const SDL_Keysym &keysym);
};

class SDLWindowBackend : public SDLBackend {
private:
  SDL_Window *window;

public:
  SDLWindowBackend(const char *window_title, int sampleRate = 44100);
  virtual ~SDLWindowBackend() override;

protected:
  SDLWindowBackend(SDL_Window *window, int sampleRate);
  virtual void endRender() override;
  virtual Event handleEvent(const SDL_Event &event) override;
};

}

#endif // TUNNELS_SDLBACKEND_H_
