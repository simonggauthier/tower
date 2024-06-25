#pragma once

#include "Editor.h"

namespace tower {
    class EditorContainer {
    public:
        EditorContainer() = default;

        virtual ~EditorContainer() {}

        virtual Editor* GetEditor() = 0;
    };
}