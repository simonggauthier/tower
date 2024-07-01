#pragma once

#include "Editor.h"

namespace tower {
    class EditorContainer {
    public:
        virtual Editor* getEditor() = 0;
    };
}
