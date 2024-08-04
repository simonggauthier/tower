#include "Folder.h"

#include <filesystem>
#include <algorithm>

#include "Debug.h"

namespace tower {
    void Folder::_parsePath() {
        for (const auto& entry : std::filesystem::directory_iterator(_path)) {
            std::wstring path = entry.path().wstring();

            path = path.replace(path.find(_path), _path.length(), L"");
            path = path.substr(1);

            _items.push_back(FolderItem(entry.is_directory() ? FolderItemTypes::FOLDER : FolderItemTypes::FILE, 
                                        path));
        }
        
        std::sort(_items.begin(), _items.end());
    }
}
