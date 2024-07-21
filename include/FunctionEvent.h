#pragma once

#include <vector>
#include <string>

#include "Event.h"

namespace tower {
    typedef std::vector<std::wstring> Arguments;

    class FunctionEvent : public Event {
    public:
        FunctionEvent() :
            Event("function"),
            _functionName(L"") {
        }
        
        void addArgument(std::wstring argument) { _arguments.push_back(argument); }

        std::wstring getFunctionName() const { return _functionName; }
        const Arguments& getArguments() const { return _arguments; }
        
        void setFunctionName(std::wstring name) { _functionName = name; }

    private:
        std::wstring _functionName;
        Arguments _arguments;    
    };
}
