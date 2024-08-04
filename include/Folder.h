#pragma once

#include <string>
#include <vector>

namespace tower {
    enum FolderItemTypes { FILE, FOLDER };

    class FolderItem {
    public:
        FolderItem(FolderItemTypes type, std::wstring path) : 
            _type(type),
            _path(path) {
        }
        
        ~FolderItem() = default;
        
        FolderItemTypes getType() const { return _type; }
        std::wstring getPath() const { return _path; }
        
        bool operator< (const FolderItem& folderItem) const {
            if (_type == FolderItemTypes::FOLDER && 
                folderItem.getType() != FOLDER) {
                return true;
            }
            
            if (folderItem.getType() == FolderItemTypes::FOLDER &&
                _type != FolderItemTypes::FOLDER) {
                return false;
            }
            
            return _path < folderItem.getPath();
        }
        
    private:
        FolderItemTypes _type;
        std::wstring _path;
    };

    class Folder {
    public:
        Folder(const std::wstring& path) : 
            _path(path) {
            _parsePath();
        }

        ~Folder() = default;

        const std::wstring getPath() const { return _path; }
        const std::vector<FolderItem> getItems() const { return _items; }
        
    private:
        void _parsePath();

        std::wstring _path;
        std::vector<FolderItem> _items;
    };
}
