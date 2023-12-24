#ifndef NODE_H
#define NODE_H

#include "uv.h"
#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <vector>

#include "../../BlockchainHeaders/Blockchain.h"

class Node {
public:


    uv_stream_t* stream;

    Node();
    Node(std::string ipAddress, int port, std::string lastBlockHash);

    std::string getIpAddress() const;

    int getPort() const;

    // joining and leaving the network
    void syncWithNetwork();
    void AnnounceLeaveNetwork();
    
    //P2P discovery methods
    void onRead(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf);
    std::string getPeersList();
    void sendPeersList(uv_stream_t* stream);
    void sendData(uv_stream_t* stream, const std::string& data);
    void listenForChanges();


private:

    std::string ipAddress;
    int port;
    std::string lastBlockHash;
    std::vector<Node> peers;
    Blockchain blockchain;
};

#endif // NODE_H