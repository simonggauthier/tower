#include "FsNode.h"

#include "Debug.h"

#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>

namespace tower {
    FsNode::FsNode(std::wstring path) :
        _path(path) {
        _parsePath();
    }
    
    FsNode::~FsNode() {
        for (auto child : _children) {            
            delete child;
        }
    }
    
    std::wstring FsNode::getName() const {
        return _path.substr(_path.find_last_of(L"\\") + 1);
    }

    void FsNode::_parsePath() {
        std::filesystem::directory_entry entry(_path);

        if (entry.is_directory()) {
            _type = FsNodeTypes::directory;
            
            _parseEntry(entry);
        } else {
            _type = FsNodeTypes::file;
        }
    }
    
    void FsNode::_parseEntry(std::filesystem::directory_entry entry) {
        for (const auto& entry : std::filesystem::directory_iterator(entry)) {
            if (entry.path().wstring().find(L".git") != std::wstring::npos) {
                continue;
            }

            std::wstring path = entry.path().wstring();
            FsNode* node = new FsNode(path);
            
            _children.push_back(node);
        }

        std::sort(_children.begin(), _children.end(), FsNodeComparator());
    }
}
