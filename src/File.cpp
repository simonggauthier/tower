#include <string>
#include <sstream>
#include <fstream>

#include "File.h"

namespace tower {
    File::File() :
        _filename(L""),
        _state(FileStates::uncreated) {
        
    }
    
    File::File(const std::wstring& filename) :
        _filename(filename),
        _state(FileStates::saved) {
    
    }
    
    std::wstring File::read() {
        std::wifstream file(_filename.c_str());
        std::wstringstream buffer;
        std::wstring line;

        while (std::getline(file, line)) {
            buffer << line << L"\r\n";
        }

        _state = FileStates::saved;

        return buffer.str();       
    }
    
    void File::write(const std::wstring& content) {
         if (_filename.empty()) {
            return;
        }

        std::wofstream file(_filename.c_str());

        if (!file.is_open()) {
            return;
        }

        for (size_t i = 0; i < content.length(); i++) {
            // ?????
            if (content[i] == 13) {
                continue;
            }

            file.put(content[i]);
        }

        file.close();
        
        _state = FileStates::saved;
    }
}
