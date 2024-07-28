#pragma once

#include "Editor.h"
#include "FunctionLine.h"

namespace tower {
    class Container {
    public:
        virtual ~Container() = default;

        virtual Editor* getEditor() = 0;
        virtual FunctionLine* getFunctionLine() = 0;
    };
}
