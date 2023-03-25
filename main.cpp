#include "system.h"

#include "SDLBackend.h"
#include "StdioFileBackend.h"
#include "classic/ScreenEngine.h"
#include "classic/SoundEngine.h"
#include "classic/DatabaseFactory.h"

int main(void) {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  Tunnels::SDLWindowBackend backend("Tunnels");
  Tunnels::Timer::TimerManager timerManager;
  Tunnels::Classic::ScreenEngine screen(backend, timerManager);
  Tunnels::Classic::SoundEngine sound(backend, timerManager);
  Tunnels::Classic::DatabaseFactory databaseFactory;
  Tunnels::StdioFileBackend file;
  Tunnels::GameEngine engine(backend, timerManager, screen, sound,
			     file, databaseFactory);
  engine.run();
  SDL_Quit();
  return EXIT_SUCCESS;
}
