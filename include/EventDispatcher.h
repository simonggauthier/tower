#pragma once

#include <vector>
#include <string>

#include "EventListener.h"
#include "Event.h"

namespace tower {
    class EventDispatcher {
    public:
        virtual ~EventDispatcher() = default;

        void addEventListener(EventListener* listener) { _listeners.push_back(listener); }
        
        std::string getEventDispatcherId() const { return _id; };
        
    protected:
        void dispatchEvent(Event* event) {
            if (event->getFromEventDispatcherId() == "") {
                event->setFromEventDispatcherId(getEventDispatcherId());
            }

            for (auto listener : _listeners) {
                listener->onEvent(event);
            }           
        }
        
        void setEventDispatcherId(std::string id) { _id = id; }
        
    private:
        std::vector<EventListener*> _listeners;
        std::string _id;
    };
}
