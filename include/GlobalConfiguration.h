#pragma once

#include <string>

#include "json.hpp"

namespace tower {
    typedef nlohmann::json config;

    class GlobalConfiguration {
    public:
        static GlobalConfiguration& getInstance() {
            static GlobalConfiguration instance;

            return instance;
        }

        ~GlobalConfiguration() = default;

        GlobalConfiguration(const GlobalConfiguration&) = delete;
        GlobalConfiguration& operator=(const GlobalConfiguration&) = delete;

        void load(const std::wstring& filename);

        const config& getConfiguration() const {
            return _config;
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
        GlobalConfiguration() = default;

        config _config;
    };
}
