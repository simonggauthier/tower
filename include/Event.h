#pragma once

#include <string>

namespace tower {
    class Event {
    public:
        Event(const std::string& name) :
            _name(name) {
            
        }
        
        std::string getName() const { return _name; }
        
    private:
        std::string _name;
    };
}
