#pragma once

#include "Event.h"

namespace tower {
    class EventListener {
    public:
        virtual void onEvent(Event* event) = 0;
    };
}
