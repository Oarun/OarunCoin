#include "../NetworkingHeaders/Node.h"
#include "../../BlockchainHeaders/Blockchain.h"
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

Node::Node(std::string ipAddress, int port, std::string lastBlockHash, const std::string& key, const std::string& iv)
    : lastBlockHash(lastBlockHash) {
    encryptedIpAddress = encrypt(ipAddress, key, iv);
    encryptedPort = encrypt(std::to_string(port), key, iv);
}

std::string Node::getIpAddress(const std::string& key, const std::string& iv) {
    return decrypt(this->encryptedIpAddress, key, iv);
}

int Node::getPort(const std::string& key, const std::string& iv) {
    return std::stoi(decrypt(encryptedPort, key, iv));
}
std::string encrypt(const std::string& plaintext, const std::string& key, const std::string& iv){
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (unsigned char*)key.c_str(), (unsigned char*)iv.c_str());

    unsigned char ciphertext[128];
    int len;
    EVP_EncryptUpdate(ctx, ciphertext, &len, (unsigned char*)plaintext.c_str(), plaintext.size());

    int ciphertext_len = len;
    EVP_EncryptFinal_ex(ctx, ciphertext + len, &len);
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return std::string((char*)ciphertext, ciphertext_len);
}

std::string decrypt(const std::string& ciphertext, const std::string& key, const std::string& iv){
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (unsigned char*)key.c_str(), (unsigned char*)iv.c_str());

    unsigned char plaintext[128];
    int len;
    EVP_DecryptUpdate(ctx, plaintext, &len, (unsigned char*)ciphertext.c_str(), ciphertext.size());

    int plaintext_len = len;
    EVP_DecryptFinal_ex(ctx, plaintext + len, &len);
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return std::string((char*)plaintext, plaintext_len);
}