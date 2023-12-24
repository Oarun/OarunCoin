#include "../NetworkingHeaders/Node.h"
#include "../NetworkingHeaders/Network.h"

Node::Node() : ipAddress(""), port(0), lastBlockHash(""){}

Node::Node(std::string ipAddress, int port, std::string lastBlockHash)
        : ipAddress(ipAddress), port(port), lastBlockHash(lastBlockHash) {}

int Node::getPort() const {
    return port;
}
std::string Node::getIpAddress() const{
    return ipAddress;
}

void Node::syncWithNetwork() {
    uv_loop_t *loop = uv_default_loop();

    // List of bootstrap nodes
    std::vector<std::pair<std::string, int>> bootstrapNodes = {
        {"73.25.5.179", 36217},
        // Add more bootstrap nodes as needed
    };

    for (const auto& bootstrapNode : bootstrapNodes) {
        std::cout << "Connecting to bootstrap node " << bootstrapNode.first << ":" << bootstrapNode.second << "..." << std::endl;

        uv_tcp_t *socket = new uv_tcp_t;
        uv_tcp_init(loop, socket);

        struct sockaddr_in dest;
        uv_ip4_addr(bootstrapNode.first.c_str(), bootstrapNode.second, &dest);

        uv_connect_t *connect_req = new uv_connect_t;
        uv_tcp_connect(connect_req, socket, (const struct sockaddr*)&dest, onConnectGetPeers);

        std::cout << "Connected to bootstrap node " << bootstrapNode.first << ":" << bootstrapNode.second << std::endl;
    }

    uv_run(loop, UV_RUN_DEFAULT);
}
void Node::AnnounceLeaveNetwork() {

}
void Node::onRead(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
    if (nread < 0) {
        // Handle error
        return;
    }

    // Convert the received data to a string
    std::string data(buf->base, nread);

    // Check if the data is a "GET_PEERS" request
    if (data.find("GET_PEERS") != std::string::npos) {
        // Get the IP address and port of the peer that is making the connection
        struct sockaddr_storage peerAddress;
        int len = sizeof(peerAddress);
        uv_tcp_getpeername((uv_tcp_t*)client, (struct sockaddr*)&peerAddress, &len);
        char ip[17] = {'\0'};
        uv_ip4_name((struct sockaddr_in*)&peerAddress, ip, sizeof(ip));
        int port = ntohs(((struct sockaddr_in*)&peerAddress)->sin_port);

        // Check if the peer is in the list of known peers
        bool isKnownPeer = false;
        for (const Node& peer : this->peers) {
            if (peer.getIpAddress() == ip && peer.getPort() == port) {
                isKnownPeer = true;
                break;
            }
        }
        // If the peer is not in the list of known peers, broadcast its information and add it to the list
        if (!isKnownPeer) {
            Node newPeer(ip, port, ""); // Assuming the last block hash is not known at this point
            std::string newPeerInfo = newPeer.getIpAddress() + ":" + std::to_string(newPeer.getPort()) + ":" + newPeer.lastBlockHash + "\n";
            for (Node& peer :  this->peers) {
                uv_stream_t* stream; // You need to get or create a uv_stream_t* for each peer
                this->sendData(stream, newPeerInfo);
            }
            this->peers.push_back(newPeer);
        }

        // Send the list of peers to the peer that made the connection
        this->sendPeersList(client);
    }
}
std::string Node::getPeersList() {
    std::string peersList = "";
    for (const Node& peer : this->peers) {
        peersList += peer.getIpAddress() + ":" + std::to_string(peer.getPort()) + ":" + peer.lastBlockHash + "\n";
    }
    // Include itself
    peersList += this->getIpAddress() + ":" + std::to_string(this->getPort()) + ":" + this->lastBlockHash + "\n";
    return peersList;
}

void Node::sendPeersList(uv_stream_t* stream) {
    std::string peersList = this->getPeersList();
    this->sendData(stream, peersList);
}
void Node::sendData(uv_stream_t* stream, const std::string& data) {
    uv_buf_t buffer = uv_buf_init((char*)data.c_str(), data.size());

    uv_write_t* writeReq = new uv_write_t;
    uv_write(writeReq, stream, &buffer, 1, [](uv_write_t* req, int status) {
        if (status) {
            std::cout << "Error sendData " << uv_strerror(status) << std::endl;
            return;
        }
        delete req;
    });
}
void Node::listenForChanges() {
    uv_read_start(stream, alloc_buffer, [](uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
        if (nread < 0) {
            if (nread != UV_EOF) {
                fprintf(stderr, "Read error: %s\n", uv_strerror(nread));
            }
            uv_close((uv_handle_t*)stream, NULL);
        } else if (nread > 0) {
            Node* node = static_cast<Node*>(stream->data);

            // Parse the new block
            std::string newBlockData(buf->base, nread);

            // Deserialize the block data into a Block object
            // This assumes you have a method Block::deserialize
            Block newBlock = Block::deserialize(newBlockData);

            // Validate the new block and add it to the blockchain
            if (node->blockchain.isValidNewBlock(newBlock)) {
                node->blockchain.chain.push_back(newBlock);
            }
        }

        // Free the buffer
        free(buf->base);
    });
}