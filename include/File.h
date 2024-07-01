#pragma once

#include <string>

namespace tower {
    enum FileStates {uncreated, saved, modified};

    class File {
    public:
        File();
        File(const std::wstring& filename);
        
        std::wstring read();
        void write(const std::wstring& content);
        
        std::wstring getFilename() const { return _filename; };
        void setFilename(const std::wstring& filename, FileStates state) { _filename = filename; _state = state; }
        
        FileStates getState() const { return _state; }
        void setState(FileStates state) { _state = state; }
        
    private:
        std::wstring _filename;
        FileStates _state;
    };
}
