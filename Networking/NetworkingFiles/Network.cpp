#include "../NetworkingHeaders/Network.h"
#include "../NetworkingHeaders/Node.h"
#include <iostream>

void onConnectGetPeers(uv_connect_t *req, int status) {
    if (status < 0) {
        // Handle error
        return;
    }

    uv_stream_t* stream = req->handle;

    uv_write_t *write_req = new uv_write_t;
    char* data = strdup("GET_PEERS");
    uv_buf_t buffer = uv_buf_init(data, strlen(data));
    write_req->data = data;
    uv_write(write_req, stream, &buffer, 1, onWriteGetPeers);
}

void onWriteGetPeers(uv_write_t *req, int status) {
    if (status) {
        std::cout << "Error onWriteGetPeers " << uv_strerror(status) << std::endl;
        return;
    }

    free(req->data);
    delete req;
}
void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    buf->base = (char*) malloc(suggested_size);
    buf->len = suggested_size;
}