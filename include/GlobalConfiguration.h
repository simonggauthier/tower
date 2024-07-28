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

    private:
        GlobalConfiguration() = default;

        config _config;
    };
}
