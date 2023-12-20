#ifndef NODE_H
#define NODE_H

#include <string>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

class Node {
public:
    std::string encryptedIpAddress;
    std::string encryptedPort;
    std::string lastBlockHash;



    Node(std::string ipAddress, int port, std::string lastBlockHash, const std::string& key, const std::string& iv);

    std::string getIpAddress(const std::string& key, const std::string& iv);

    int getPort(const std::string& key, const std::string& iv);
    

private:
    std::string encrypt(const std::string& plaintext, const std::string& key, const std::string& iv);

    std::string decrypt(const std::string& ciphertext, const std::string& key, const std::string& iv);
};

#endif // NODE_H