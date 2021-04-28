/*
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

/*  * * * * * * * * * * * * * * * * * * * * * * * * * * *
 Code by Simon Monk
 http://www.simonmonk.org
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef Event_h
#define Event_h

#include <inttypes.h>

#define EVENT_NONE 0
#define EVENT_EVERY 1
#define EVENT_OSCILLATE 2

class Event
{

public:
  Event(void);
  void update(void);
  void update(unsigned long now);
  int8_t eventType;
  unsigned long period;
  int repeatCount;
  uint8_t pin;
  uint8_t pinState;
  void (*callback)(void);
  unsigned long lastEventTime;
  int count;
};

#endif

#ifndef Timer_h
#define Timer_h

#include <inttypes.h>


#define MAX_NUMBER_OF_EVENTS (10)

#define TIMER_NOT_AN_EVENT (-2)
#define NO_TIMER_AVAILABLE (-1)

class Timer
{

public:
  Timer(void);

  int8_t every(unsigned long period, void (*callback)(void));
  int8_t every(unsigned long period, void (*callback)(void), int repeatCount);
  int8_t after(unsigned long duration, void (*callback)(void));
  int8_t oscillate(uint8_t pin, unsigned long period, uint8_t startingValue);
  int8_t oscillate(uint8_t pin, unsigned long period, uint8_t startingValue, int repeatCount);
  
  /**
   * This method will generate a pulse of !startingValue, occuring period after the
   * call of this method and lasting for period. The Pin will be left in !startingValue.
   */
  int8_t pulse(uint8_t pin, unsigned long period, uint8_t startingValue);
  
  /**
   * This method will generate a pulse of pulseValue, starting immediately and of
   * length period. The pin will be left in the !pulseValue state
   */
  int8_t pulseImmediate(uint8_t pin, unsigned long period, uint8_t pulseValue);
  void stop(int8_t id);
  void update(void);
  void update(unsigned long now);
  

protected:
  Event _events[MAX_NUMBER_OF_EVENTS];
  int8_t findFreeEventIndex(void);

};
#endif

//// YOUR CODE STARTS HERE
Timer t;
int S3 = 13;
int S2 = 12;
int S1 = 11;
int S0 = 10;
int up0;
int up1;
int up2;
int up3;
int d0;
int d1;
int d2;
int d3;

void setup() {
    Serial.begin(9600);
  pinMode(S3, OUTPUT);
  pinMode(S2, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S0, OUTPUT);
  
  
    // starts from high, oscillates on the pin every 500ms
    up0 = t.oscillate(S0, 1000, LOW);
    up1 = t.oscillate(S1, 2000, LOW);
    up2 = t.oscillate(S2, 4000, LOW);
    up3 = t.oscillate(S3, 8000, LOW);
    
    t.every(15000,Godown);
    t.every(30000,Goup);   
    }

// 1 unit of your timer = 500ms in real time

void loop() {
  t.update();
}
void Godown() {
  t.stop(up0);
  t.stop(up1);
  t.stop(up2);
  t.stop(up3);
  d0 = t.oscillate(S0,1000,HIGH);
  d1 = t.oscillate(S1,2000,HIGH);
  d2 = t.oscillate(S2,4000,HIGH);
  d3 = t.oscillate(S3,8000,HIGH);
    
  
}
void Goup(){
  t.stop(d0);
  t.stop(d1);
  t.stop(d2);
  t.stop(d3);
  up0 = t.oscillate(S0, 1000, LOW);
  up1 = t.oscillate(S1, 2000, LOW);
  up2 = t.oscillate(S2, 4000, LOW);
  up3 = t.oscillate(S3, 8000, LOW);
  
}
// "every" X milliseconds


//// YOUR CODE ENDS HERE

// For Arduino 1.0 and earlier
// #if defined(ARDUINO) && ARDUINO >= 100
// #include "Arduino.h"
// #else
// #include "WProgram.h"
// #endif

Event::Event(void)
{
  eventType = EVENT_NONE;
}

void Event::update(void)
{
    unsigned long now = millis();
    update(now);
}

void Event::update(unsigned long now)
{
  if (now - lastEventTime >= period)
  {
    switch (eventType)
    {
      case EVENT_EVERY:
        (*callback)();
        break;

      case EVENT_OSCILLATE:
        pinState = ! pinState;
        digitalWrite(pin, pinState);
        break;
    }
    lastEventTime = now;
    count++;
  }
  if (repeatCount > -1 && count >= repeatCount)
  {
    eventType = EVENT_NONE;
  }
}


Timer::Timer(void)
{
}

int8_t Timer::every(unsigned long period, void (*callback)(), int repeatCount)
{
  int8_t i = findFreeEventIndex();
  if (i == -1) return -1;

  _events[i].eventType = EVENT_EVERY;
  _events[i].period = period;
  _events[i].repeatCount = repeatCount;
  _events[i].callback = callback;
  _events[i].lastEventTime = millis();
  _events[i].count = 0;
  return i;
}

int8_t Timer::every(unsigned long period, void (*callback)())
{
  return every(period, callback, -1); // - means forever
}

int8_t Timer::after(unsigned long period, void (*callback)())
{
  return every(period, callback, 1);
}

int8_t Timer::oscillate(uint8_t pin, unsigned long period, uint8_t startingValue, int repeatCount)
{
  int8_t i = findFreeEventIndex();
  if (i == NO_TIMER_AVAILABLE) return NO_TIMER_AVAILABLE;

  _events[i].eventType = EVENT_OSCILLATE;
  _events[i].pin = pin;
  _events[i].period = period;
  _events[i].pinState = startingValue;
  digitalWrite(pin, startingValue);
  _events[i].repeatCount = repeatCount * 2; // full cycles not transitions
  _events[i].lastEventTime = millis();
  _events[i].count = 0;
  return i;
}

int8_t Timer::oscillate(uint8_t pin, unsigned long period, uint8_t startingValue)
{
  return oscillate(pin, period, startingValue, -1); // forever
}

/**
 * This method will generate a pulse of !startingValue, occuring period after the
 * call of this method and lasting for period. The Pin will be left in !startingValue.
 */
int8_t Timer::pulse(uint8_t pin, unsigned long period, uint8_t startingValue)
{
  return oscillate(pin, period, startingValue, 1); // once
}

/**
 * This method will generate a pulse of startingValue, starting immediately and of
 * length period. The pin will be left in the !startingValue state
 */
int8_t Timer::pulseImmediate(uint8_t pin, unsigned long period, uint8_t pulseValue)
{
  int8_t id(oscillate(pin, period, pulseValue, 1));
  // now fix the repeat count
  if (id >= 0 && id < MAX_NUMBER_OF_EVENTS) {
    _events[id].repeatCount = 1;
  }
  return id;
}


void Timer::stop(int8_t id)
{
  if (id >= 0 && id < MAX_NUMBER_OF_EVENTS) {
    _events[id].eventType = EVENT_NONE;
  }
}

void Timer::update(void)
{
  unsigned long now = millis();
  update(now);
}

void Timer::update(unsigned long now)
{
  for (int8_t i = 0; i < MAX_NUMBER_OF_EVENTS; i++)
  {
    if (_events[i].eventType != EVENT_NONE)
    {
      _events[i].update(now);
    }
  }
}
int8_t Timer::findFreeEventIndex(void)
{
  for (int8_t i = 0; i < MAX_NUMBER_OF_EVENTS; i++)
  {
    if (_events[i].eventType == EVENT_NONE)
    {
      return i;
    }
  }
  return NO_TIMER_AVAILABLE;
}
