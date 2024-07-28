#pragma once

#include "Event.h"

namespace tower {
    class EventListener {
    public:
        virtual ~EventListener() = default;

        virtual void onEvent(Event* event) = 0;
    };
}
