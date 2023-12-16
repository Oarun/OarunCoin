#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <vector>
#include <ctime>
#include <string>

struct TransactionData
{
    std::string senderKey;
    std::string receiverKey;
    double amount;
    time_t timestamp;
};

class Block
{
public:
    std::string prevHash;

    TransactionData data;
    double stakeAmount;

    Block(int idx, TransactionData d, std::string prevHash);

    // Get original hash
    std::string getHash() const;

    // Get previous hash
    std::string getPrevHash();

    // Validate hash
    bool isHashValid();
    int getIndex() const;

    Block(int idx, TransactionData d, std::string prevHash);

private:
    int index;
    std::string blockHash;
    std::string generateHash();
};

class Blockchain
{
public:
    // Public chain
    std::vector<Block> chain;

    // Add a new block to the chain
    void addBlock(TransactionData data);

    // Constructor
    Blockchain();
    void OuroborosProtocol(); // Proof of Stake
    void saveChain();
    void loadChain();

private:
    int chainLength;

    // Check if chain is valid
    bool isChainValid();

    // Return latest block on the chain
    Block *getLatestBlock();

    //Creates the first block in the chain
    Block createGenesisBlock();
};

#endif // BLOCKCHAIN_H