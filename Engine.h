#ifndef TUNNELS_ENGINE_H_
#define TUNNELS_ENGINE_H_

#include "EventLoop.h"

namespace Tunnels {

namespace Timer { class TimerManager; }

class ScreenEngine {
public:
  virtual ~ScreenEngine() {}
  virtual void refresh() {}
  virtual void drawTitleScreen() = 0;
  virtual void initMenu() = 0;
  virtual void drawPrompt(unsigned n) = 0;
  virtual void setCursorEnabled(bool enabled) = 0;
  virtual void markSelection(byte ch) = 0;
  virtual void prepareStringInputField(unsigned len) = 0;
  virtual void endStringInputField(unsigned len, unsigned cnt) = 0;
  virtual void addStringInputField(byte ch) = 0;
  virtual void eraseStringInputField(unsigned cnt) = 0;
};

class SoundEngine {
public:
  virtual ~SoundEngine() {}
  virtual void stopMusic() = 0;
  virtual void playTitleMusic() = 0;
  virtual void beep() = 0;
  virtual void honk() = 0;
};

class GameEngine {
private:
  EventLoop &eventLoop;
  Timer::TimerManager &timerManager;
  ScreenEngine &screen;
  SoundEngine &sound;

  enum Checkpoint {
    CHECKPOINT_NULL,           // keep going
    CHECKPOINT_LOAD_SAVE,      // G@>6046
    CHECKPOINT_NEW_OR_RESTOCK, // G@>6114

    // Meta checkpoints
    CHECKPOINT_AID,
    CHECKPOINT_UP,
    CHECKPOINT_DOWN,
    CHECKPOINT_QUIT
  };
  Checkpoint redoTarget;
  Checkpoint backTarget;
  Checkpoint procdTarget;

  enum {
    ACCEPT_PROCD         = 0x01,
    ACCEPT_BACK          = 0x02,
    ACCEPT_REDO          = 0x04,
    ACCEPT_AID           = 0x08,
    ACCEPT_UP_DOWN       = 0x10,
    ACCEPT_NUMERIC       = 0x20,
    ACCEPT_ALPHANUMERIC  = 0x40,
    ACCEPT_ALPHANUMERIC2 = 0x80
  };
  byte acceptMask;

  byte progression;

public:
  GameEngine(EventLoop &eventLoop_, Timer::TimerManager &timerManager_,
	     ScreenEngine &screen_, SoundEngine &sound_);
  EventType run();

private:
  Event nextEvent();
  EventType waitForEvent();
  Checkpoint titleScreen();
  Checkpoint loadSave();
  Checkpoint getKeyNoCursor(byte &kc);
  Checkpoint getKey(byte &kc);
  Checkpoint getNumberKey(byte &n, byte low, byte high);
  Checkpoint getString(unsigned len, byte *result = nullptr);
  template<unsigned n> Checkpoint getString(byte (&result)[n])
  { return getString(n, &result[0]); }
};

}

#endif // TUNNELS_ENGINE_H_
