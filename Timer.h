#ifndef TUNNELS_TIMER_H_
#define TUNNELS_TIMER_H_

#include "EventLoop.h"

namespace Tunnels { namespace Timer {

class TimerManager;

class Timer {
  friend class TimerManager;
 private:
  TimerManager *manager;
  Timer *next, *prev;
  uint32 expiry;
 protected:
  Timer() : manager(nullptr) { }
  uint32 getExpiry() const { return expiry; }
  virtual void callback() = 0;
 public:
  virtual ~Timer();
};

class TimerManager : Timer {
 private:
  Timer *first, *immediate, *deferred;
  Event timerEvent;
 public:
  TimerManager();
  ~TimerManager();
  void addTimer(Timer& timer);
  void addTimer(Timer& timer, uint32 expiry);
  void addTimerAfter(Timer& timer, uint32 delay);
  void removeTimer(Timer& timer);
  uint32 getNextDelay(uint32 now);
  Event pollTimerEvent();
  void setTimeout(uint32 expiry) { addTimer(*this, expiry); }
  void setTimeoutAfter(uint32 delay) { addTimerAfter(*this, delay); }
  void removeTimeout() { removeTimer(*this); }
  void postTimerEvent(Event &&e);
private:
  bool checkFirstTimer(uint32 now);
protected:
  virtual void callback() override;
};

}}

#endif // TUNNELS_TIMER_H_
