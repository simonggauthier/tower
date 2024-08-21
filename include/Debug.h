#pragma once

#include <iostream>

#include "FsNode.h"

#define _TOWER_DEBUG(msg) std::cout << msg << std::endl;
#define _TOWER_DEBUGW(msg) std::wcout << msg << std::endl;

namespace tower {
    void _TOWER_DEBUG_PRINT_FSNODE(FsNode* fsNode);
    void _TOWER_DEBUG_PRINT_FSNODE_R(FsNode* fsNode, int depth);
}
