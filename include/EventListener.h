#pragma once

namespace tower {
    class EventListener {
    public:
        virtual void onEvent() = 0;
    };
}
