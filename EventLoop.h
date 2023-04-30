#ifndef TUNNELS_EVENTLOOP_H_
#define TUNNELS_EVENTLOOP_H_

namespace Tunnels {

namespace Timer { class TimerManager; }

enum EventType {
  EVENT_NULL,
  EVENT_QUIT,
  EVENT_TIMEOUT,
  EVENT_KEY,
  EVENT_END_OF_MUSIC
};

enum KeyCodes {
  KEY_AID = 1,
  KEY_CLEAR = 2,
  KEY_DEL = 3,
  KEY_INS = 4,
  KEY_QUIT = 5,
  KEY_REDO = 6,
  KEY_ERASE = 7,
  KEY_LEFT = 8,
  KEY_RIGHT = 9,
  KEY_DOWN = 10,
  KEY_UP = 11,
  KEY_PROCD = 12,
  KEY_ENTER = 13,
  KEY_BEGIN = 14,
  KEY_BACK = 15
};

class Event {
private:
  uint32 eventNum;
  Event(EventType t, uint32 n = 0) :
    eventNum((n<<4)|static_cast<uint32>(t)) {}
  uint32 value() const { return eventNum >> 4; }

public:
  EventType type() const { return static_cast<EventType>(eventNum & 15); }
  byte keycode() const { return (type() == EVENT_KEY? value() : 0); }

  bool operator==(const Event &b) const { return b.eventNum == eventNum; }
  bool operator!=(const Event &b) const { return b.eventNum != eventNum; }
  operator bool() const { return eventNum != EVENT_NULL; }
  
  static Event keyEvent(byte key) { return Event(EVENT_KEY, key); }
  static Event quitEvent() { return Event(EVENT_QUIT); }
  static Event timeoutEvent() { return Event(EVENT_TIMEOUT); }
  static Event nullEvent() { return Event(EVENT_NULL); }
  static Event endOfMusicEvent() { return Event(EVENT_END_OF_MUSIC); }
};

class EventLoop {
 public:
  virtual Event runEventLoop(Timer::TimerManager& timerManager) = 0;
};

}

#endif // TUNNELS_EVENTLOOP_H_
