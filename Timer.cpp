#include "system.h"

#include "Timer.h"

namespace Tunnels { namespace Timer {

Timer::~Timer()
{
  if (manager)
    manager->removeTimer(*this);
}

TimerManager::TimerManager() :
  first(nullptr), immediate(nullptr), deferred(nullptr),
  timeoutHasExpired(false)
{  
}

TimerManager::~TimerManager()
{
  Timer *t;
  while ((t = immediate)) {
    immediate = t->next;
    t->next = nullptr;
    t->prev = nullptr;
    t->manager = nullptr;
  }
  while ((t = deferred)) {
    deferred = t->next;
    t->next = nullptr;
    t->prev = nullptr;
    t->manager = nullptr;
  }
  while ((t = first)) {
    first = t->next;
    t->next = nullptr;
    t->prev = nullptr;
    t->manager = nullptr;
  }
}

void TimerManager::addTimer(Timer& timer)
{
  if (timer.manager)
    timer.manager->removeTimer(timer);
  if (timer.manager == nullptr) {
    timer.prev = nullptr;
    timer.next = immediate;
    if (immediate)
      immediate->prev = &timer;
    immediate = &timer;
    timer.manager = this;
  }
}

void TimerManager::addTimer(Timer& timer, uint32 expiry)
{
  if (timer.manager)
    timer.manager->removeTimer(timer);
  if (timer.manager == nullptr) {
    Timer *tprev = nullptr, *tnext;
    while((tnext = (tprev? tprev->next : first)))
      if (((int32)(expiry - tnext->expiry)) < 0)
	break;
      else
	tprev = tnext;
    timer.expiry = expiry;
    timer.prev = tprev;
    timer.next = tnext;
    if (tprev)
      tprev->next = &timer;
    else
      first = &timer;
    if (tnext)
      tnext->prev = &timer;
    timer.manager = this;
  }
}

void TimerManager::addTimerAfter(Timer& timer, uint32 delay)
{
  if (!delay) {
    addTimer(timer);
    return;
  }
  if (timer.manager)
    timer.manager->removeTimer(timer);
  if (timer.manager == nullptr) {
    timer.expiry = delay;
    timer.prev = nullptr;
    timer.next = deferred;
    if (deferred)
      deferred->prev = &timer;
    deferred = &timer;
    timer.manager = this;
  }
}

void TimerManager::removeTimer(Timer& timer)
{
  if (timer.manager == this) {
    if (timer.prev)
      timer.prev->next = timer.next;
    else if(first == &timer)
      first = timer.next;
    else if(immediate == &timer)
      immediate = timer.next;
    else if(deferred == &timer)
      deferred = timer.next;
    if (timer.next)
      timer.next->prev = timer.prev;
    timer.next = nullptr;
    timer.prev = nullptr;
    timer.manager = nullptr;
  }
}

uint32 TimerManager::getNextDelay(uint32 now)
{
  while (checkFirstTimer(now))
    ;
  if (!first)
    return 0;
  return first->expiry - now;
}

bool TimerManager::timeoutExpired()
{
  if (!timeoutHasExpired)
    return false;
  timeoutHasExpired = false;
  return true;
}

bool TimerManager::checkFirstTimer(uint32 now)
{
  Timer *t;
  while ((t = deferred))
    addTimer(*t, now+t->expiry);
  if ((t = immediate))
    t->expiry = now;
  else if (!(t = first) || ((int32)(t->expiry - now))>0)
    return false;
  removeTimer(*t);
  t->callback();
  return true;
}

void TimerManager::callback()
{
  timeoutHasExpired = true;
}

}}
