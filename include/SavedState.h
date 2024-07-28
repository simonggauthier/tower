#pragma once

#include <string>

#include "json.hpp"

namespace tower {
    typedef nlohmann::json state;

    class SavedState {
    public:
        static SavedState& getInstance() {
            static SavedState instance;

            return instance;
        }

        ~SavedState() = default;

        SavedState(const SavedState&) = delete;
        SavedState& operator=(const SavedState&) = delete;

        void load(const std::wstring& filename);

        void save(const std::wstring& filename);

        state& getState() {
            return _state;
        }

        static std::string wideToNarrow(const std::wstring& wide) {
            std::string narrow(wide.begin(), wide.end());
            
            return narrow;            
        }

        static std::wstring narrowToWide(const std::string& narrow) {
            std::wstring wide(narrow.begin(), narrow.end());
            
            return wide;
        }

    private:
        SavedState() = default;

        state _state;
    };
}
