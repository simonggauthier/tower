#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace tower {
    enum class FsNodeTypes {
        directory,
        file
    };

    class FsNode {
    public:
        FsNode(std::wstring path);
        
        ~FsNode();
        
        std::wstring getPath() const { return _path; }
        FsNodeTypes getType() const { return _type; }

        std::vector<FsNode*>::iterator childrenBegin() { return _children.begin(); }
        std::vector<FsNode*>::iterator childrenEnd() { return _children.end(); }

        std::wstring getName() const;

        int countAllNodes();

    private:
        void _parsePath();
        void _parseEntry(std::filesystem::directory_entry entry);
        int _countAllNodes(FsNode* fsNode, int total);

        std::wstring _path;
        FsNodeTypes _type;
        
        std::vector<FsNode*> _children;

        int _countCache;
    };

    struct FsNodeComparator
    {
        bool operator()(FsNode* n1, FsNode* n2) {
            if (n1->getType() == FsNodeTypes::directory &&
                n2->getType() == FsNodeTypes::file) {
                return true;
            }

            if (n1->getType() == FsNodeTypes::file &&
                n2->getType() == FsNodeTypes::directory) {
                return false;
            }

            return n1->getPath() < n2->getPath();
        }
    };
}
