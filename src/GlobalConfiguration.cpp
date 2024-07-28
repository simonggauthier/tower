#include <fstream>

#include "json.hpp"

#include "GlobalConfiguration.h"

namespace tower {
    void GlobalConfiguration::load(const std::wstring& filename) {
        std::ifstream cstream(filename.c_str());
        _config = config::parse(cstream);
    }
}
