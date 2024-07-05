#pragma once

#include <vector>

#include "EventListener.h"
#include "Event.h"

namespace tower {
    class EventDispatcher {
    public:        
        void addEventListener(EventListener* listener) { _listeners.push_back(listener); }
        
    protected:
        void dispatchEvent(Event* event) {
            for (auto listener : _listeners) {
                listener->onEvent(event);
            }           
        }
        
    private:
        std::vector<EventListener*> _listeners;
    };
}
