#include <fstream>
#include <string>

#include "json.hpp"

#include "SavedState.h"
#include "Debug.h"

namespace tower {
    void SavedState::load(const std::wstring& filename) {
        std::ifstream cstream(filename.c_str());
        
        if (cstream.fail()) {
            return;
        }
        
        _state = state::parse(cstream);
    }

    void SavedState::save(const std::wstring& filename) {
        std::wofstream cstream(filename.c_str());

        _TOWER_DEBUGW(L"Saving state to " << filename << std::endl);

        std::string json = _state.dump(4);
        std::wstring tmp(json.begin(), json.end());

        cstream << tmp;
    }
}
