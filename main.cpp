#include <uv.h>
#include <iostream>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <fstream>
#include <vector>

#include "BlockchainHeaders/Blockchain.h"
#include "Networking/NetworkingHeaders/Network.h"
#include "Networking/NetworkingHeaders/Node.h"


void onReadListener(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
    if (nread < 0) {
        if (nread != UV_EOF) {
            fprintf(stderr, "Read error: %s\n", uv_strerror(nread));
        }
        uv_close((uv_handle_t*) stream, NULL);
    } else if (nread > 0) {
        // Process the data that was read...
    }

    // Don't forget to free the read buffer
    if (buf->base) {
        free(buf->base);
    }
}
uv_stream_t* initializeNetworkStream() {
    uv_tcp_t* tcp = new uv_tcp_t;
    uv_tcp_init(uv_default_loop(), tcp);

    struct sockaddr_in addr;
    uv_ip4_addr("0.0.0.0", 36217, &addr); // Listen on all interfaces

    uv_tcp_bind(tcp, (const struct sockaddr*)&addr, 0);

    int r = uv_listen((uv_stream_t*) tcp, 128, [](uv_stream_t* server, int status) {
        if (status < 0) {
            fprintf(stderr, "Listen error: %s\n", uv_strerror(status));
            return;
        }

        uv_tcp_t* client = new uv_tcp_t;
        uv_tcp_init(uv_default_loop(), client);
        if (uv_accept(server, (uv_stream_t*) client) == 0) {
            client->data = server->data;
            uv_read_start((uv_stream_t*) client, alloc_buffer, onReadListener);
        }
        else {
            uv_close((uv_handle_t*) client, NULL);
        }
    });

    if (r) {
        fprintf(stderr, "Listen error: %s\n", uv_strerror(r));
        return nullptr;
    }

    return (uv_stream_t*)tcp;
}

int main() {
    std::cout << "Initialization of blockchain..." << std::endl;

    Blockchain* blockchain = new Blockchain();

    std::cout << "Genesis block is created locally..." << std::endl;

    Node* localNode = new Node();

    std::cout << "Local node is created..." << std::endl;

    uv_loop_t *loop = uv_default_loop();


    uv_stream_t* stream = initializeNetworkStream();
    localNode->stream = stream;
    stream->data = localNode;

    uv_timer_t timer;
    uv_timer_init(loop, &timer);

    uv_timer_start(&timer, [](uv_timer_t* timer) {
        Node* localNode = static_cast<Node*>(timer->data);
        localNode->syncWithNetwork();
        localNode->listenForChanges();
    }, 0, 5000);


    timer.data = localNode;

    uv_run(loop, UV_RUN_DEFAULT);

    delete localNode;
    delete blockchain;
}