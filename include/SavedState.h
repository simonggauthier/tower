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

        const state& getState() const {
            return _state;
        }

    private:
        SavedState() = default;

        state _state;
    };
}
