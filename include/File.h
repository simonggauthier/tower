#pragma once

#include <string>

namespace tower {
    enum FileStates {saved, dirty};

    class File {
    public:
        File();
        File(const std::wstring& filename);
        ~File();
        
        std::wstring Read() const;
        void Write(const std::wstring& content);
        
        std::wstring GetFilename() const { return _filename; };
        FileStates GetState() const { return _state; }
        void SetState(FileStates state) { _state = state; }
        
    private:
        std::wstring _filename;
        FileStates _state;
    };
}
