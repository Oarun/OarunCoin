#include <uv.h>
#include <iostream>
#include "BlockchainHeaders/Blockchain.h"
#include "Networking/NetworkingHeaders/Network.h"

int main() {

    uv_loop_t *loop = uv_default_loop();

    // Create a new Blockchain
    Blockchain blockchain;

    // Listen for incoming connections
    uv_tcp_t server;
    uv_tcp_init(loop, &server);

    struct sockaddr_in addr;
    uv_ip4_addr("0.0.0.0", 36217, &addr);

    uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);
    int r = uv_listen((uv_stream_t*) &server, 128, on_new_connection);
    if (r) {
        std::cerr << "Listen error: " << uv_strerror(r) << std::endl;
        return 1;
    }

    return uv_run(loop, UV_RUN_DEFAULT);

    return 0;
}