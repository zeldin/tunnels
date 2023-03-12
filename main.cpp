#include "system.h"

#include "SDLBackend.h"
#include "classic/ScreenEngine.h"
#include "classic/SoundEngine.h"

int main(void) {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  Tunnels::SDLWindowBackend backend("Tunnels");
  Tunnels::Timer::TimerManager timerManager;
  Tunnels::Classic::ScreenEngine screen(backend);
  Tunnels::Classic::SoundEngine sound(backend, timerManager);
  screen.drawTitleScreen();
  sound.playTitleMusic();
  screen.refresh();
  backend.runEventLoop(timerManager);
  sound.stopMusic();
  timerManager.removeTimeout();
  SDL_Quit();
  return EXIT_SUCCESS;
}
