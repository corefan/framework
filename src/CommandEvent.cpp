#include <CommandEvent.h>
#include <Element.h>

void
CommandEvent::dispatch(Element & element) {
  if (!isHandled()) {
    element.onCommandEvent(*this);
  }
  EventBase::dispatch(element);
}
