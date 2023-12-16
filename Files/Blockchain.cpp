#include "../Headers/Blockchain.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <openssl/sha.h>
#include <assert.h>

Block::Block(int idx, TransactionData d, std::string prevHash)
{
    stakeAmount = 0.0;
    index = idx;
    data = d;
    this->prevHash = prevHash;
    blockHash = generateHash();
}

std::string Block::getHash()
{
    return blockHash;
}

std::string Block::getPrevHash()
{
    return prevHash;
}

bool Block::isHashValid()
{
    return generateHash() == blockHash;
}

std::string Block::generateHash()
{
    std::stringstream ss;
    ss << index << prevHash << data.timestamp << data.amount << data.receiverKey << data.senderKey;

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, ss.str().c_str(), ss.str().size());
    SHA256_Final(hash, &sha256);

    std::stringstream sha;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sha << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return sha.str();
}

Blockchain::Blockchain()
{
    Block genesis = createGenesisBlock();
    chain.push_back(genesis);
}

Block Blockchain::createGenesisBlock()
{
    time_t current;
    TransactionData d;
    d.amount = 0;
    d.receiverKey = "None";
    d.senderKey = "None";
    d.timestamp = time(&current);

    std::hash<std::string> hash1;
    std::string hash2 = std::to_string(hash1("Genesis"));
    Block genesis(0, d, hash2);

    return genesis;
}

Block *Blockchain::getLatestBlock()
{
    return &chain.back();
}

void Blockchain::addBlock(TransactionData d)
{
    int index = (int)chain.size();
    std::string previousHash = (this->getLatestBlock())->getHash();
    
    Block newBlock(index, d, previousHash);
    chain.push_back(newBlock);
}

bool Blockchain::isChainValid()
{
    std::vector<Block>::iterator it;

    for (it = chain.begin(); it != chain.end(); ++it)
    {
        Block currentBlock = *it;
        if (!currentBlock.isHashValid())
        {
            return false;
        }

        if (it != chain.begin())
        {
            Block previousBlock = *(it - 1);
            if (currentBlock.prevHash != previousBlock.getHash())
            {
                return false;
            }
        }
    }

    return true;
}
void Blockchain::OuroborosProtocol(){
    std::vector<Block>::iterator it;
    double totalStake = 0.0;
    for (it = chain.begin(); it != chain.end(); ++it)
    {
        totalStake += it->stakeAmount;
    }

    double lottery = (double)rand() / RAND_MAX * totalStake;

    for (it = chain.begin(); it != chain.end(); ++it)
    {
        if (it->stakeAmount >= lottery)
        {
            // This block is the winner and can create the next block.
            break;
        }
        lottery -= it->stakeAmount;
    }

    assert(it != chain.end());
}