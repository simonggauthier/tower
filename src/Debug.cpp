#include "Debug.h"

#include <string>
#include <iterator>

#include "FsNode.h"

namespace tower {
    void _TOWER_DEBUG_PRINT_FSNODE(FsNode* fsNode) {
        _TOWER_DEBUG_PRINT_FSNODE_R(fsNode, 0);
    }

    void _TOWER_DEBUG_PRINT_FSNODE_R(FsNode* fsNode, int depth) {
        std::wstring prefix(depth * 2, L' ');

        _TOWER_DEBUGW(prefix << fsNode->getPath());

        if (fsNode->getType() == FsNodeTypes::directory && 
            fsNode->getPath().find(L".git") == std::wstring::npos) {
            auto iterator = fsNode->childrenBegin();

            while (iterator != fsNode->childrenEnd()) {
                _TOWER_DEBUG_PRINT_FSNODE_R(*iterator, depth + 1);

                iterator = std::next(iterator, 1);
            }
        }
    }
}
