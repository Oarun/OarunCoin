#include "../BlockchainHeaders/Blockchain.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <openssl/evp.h>
#include <assert.h>
#include <fstream>

Block::Block(int idx, TransactionData d, std::string prevHash)
{
    stakeAmount = 0.0;
    index = idx;
    data = d;
    this->prevHash = prevHash;
    blockHash = generateHash();
}

std::string Block::getHash() const
{
    return blockHash;
}

std::string Block::getPrevHash()
{
    return prevHash;
}
int Block::getIndex() const
{
    return index;
}
bool Block::isHashValid()
{
    return generateHash() == blockHash;
}

std::string Block::generateHash()
{
    std::stringstream ss;
    ss << index << prevHash << data.timestamp << data.amount << data.receiverKey << data.senderKey;

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int lengthOfHash = 0;

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(ctx, ss.str().c_str(), ss.str().size());
    EVP_DigestFinal_ex(ctx, hash, &lengthOfHash);
    EVP_MD_CTX_free(ctx);

    std::stringstream sha;
    for(unsigned int i = 0; i < lengthOfHash; i++)
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

void Blockchain::saveChain()
{
    std::ofstream file("blockchain.dat");
    for (const Block& block : chain) {
        file << block.prevHash << std::endl;
        file << block.data.senderKey << std::endl;
        file << block.data.receiverKey << std::endl;
        file << block.data.amount << std::endl;
        file << block.data.timestamp << std::endl;
        file << block.stakeAmount << std::endl;
        file << block.getIndex() << std::endl;
        file << block.getHash() << std::endl;
    }
}
void Blockchain::loadChain()
{
    std::ifstream file("blockchain.dat");
    std::string prevHash;
    std::string senderKey;
    std::string receiverKey;
    double amount;
    time_t timestamp;
    double stakeAmount;
    int index;
    std::string hash;
    while (file >> prevHash >> senderKey >> receiverKey >> amount >> timestamp >> stakeAmount >> index >> hash) {
        TransactionData data;
        data.senderKey = senderKey;
        data.receiverKey = receiverKey;
        data.amount = amount;
        data.timestamp = timestamp;
        Block block(index, data, prevHash);
        block.stakeAmount = stakeAmount;
        chain.push_back(block);
    }
}
Block Blockchain::getLastBlock() const
{
    return chain.back();
}