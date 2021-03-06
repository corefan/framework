#ifndef _COMMANDEVENT_H_
#define _COMMANDEVENT_H_

#include <EventBase.h>

class CommandEvent : public EventBase {
public:
  CommandEvent(double _timestamp, int _element_id, int _value = 0)
    : EventBase(_timestamp), element_id(_element_id), value(_value) { }

  void dispatch(Element & element) override;

  int getElementId() const { return element_id; }
  int getValue() const { return value; }
  
 private:
  int element_id, value;
  int edge = +1;
};

#endif
