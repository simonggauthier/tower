#pragma once

#include "FunctionLine.h"

namespace tower {
    class FunctionLineContainer {
    public:
        virtual FunctionLine* getFunctionLine() = 0;
    };
}
