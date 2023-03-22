#include "system.h"

#include "SDLBackend.h"
#include "classic/ScreenEngine.h"
#include "classic/SoundEngine.h"

int main(void) {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  Tunnels::SDLWindowBackend backend("Tunnels");
  Tunnels::Timer::TimerManager timerManager;
  Tunnels::Classic::ScreenEngine screen(backend, timerManager);
  Tunnels::Classic::SoundEngine sound(backend, timerManager);
  Tunnels::GameEngine engine(backend, timerManager, screen, sound);
  engine.run();
  SDL_Quit();
  return EXIT_SUCCESS;
}
