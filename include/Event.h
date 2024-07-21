#pragma once

#include <string>

namespace tower {
    class Event {
    public:
        Event(const std::string& name, const std::string& fromEventDispatcherId) :
            _name(name),
            _fromEventDispatcherId(fromEventDispatcherId) {
        }
        
        Event(const std::string& name) :
            _name(name),
            _fromEventDispatcherId("") {
        }

        virtual ~Event() = default;

        std::string getName() const { return _name; }
        std::string getFromEventDispatcherId() const { return _fromEventDispatcherId; }
        
        void setFromEventDispatcherId(std::string fromEventDispatcherId) { _fromEventDispatcherId = fromEventDispatcherId; }

    private:
        std::string _name;
        std::string _fromEventDispatcherId;
    };
}
