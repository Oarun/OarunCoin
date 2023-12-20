#ifndef NODEMANAGER_H
#define NODEMANAGER_H

#include "Node.h"
#include "../../BlockchainHeaders/Blockchain.h"
#include <vector>

class NodeManager {
public:
    std::vector<Node> nodes;

    bool isNodeUpToDate(const Node& node, const Blockchain& blockchain) {
        return node.lastBlockHash == blockchain.getLastBlock().getHash();
    }
};

#endif // NODEMANAGER_H