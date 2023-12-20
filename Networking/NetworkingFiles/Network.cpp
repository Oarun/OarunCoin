#include "../NetworkingHeaders/Network.h"
#include <iostream>

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    *buf = uv_buf_init((char*) malloc(suggested_size), suggested_size);
}

void on_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
    if (nread < 0){
        if (nread != UV_EOF)
            fprintf(stderr, "Read error: %s\n", uv_err_name(nread));
        uv_close((uv_handle_t*) client, NULL);
    } else if (nread > 0) {
        // TODO: Parse buf->base into a Block object and validate it
    }

    if (buf->base)
        free(buf->base);
}

void on_new_connection(uv_stream_t *server, int status) {
    if (status < 0) {
        std::cerr << "New connection error: " << uv_strerror(status) << std::endl;
        return;
    }

    uv_tcp_t *client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
    uv_tcp_init(uv_default_loop(), client);

    if (uv_accept(server, (uv_stream_t*) client) == 0) {
        uv_read_start((uv_stream_t*) client, alloc_buffer, on_read);
    }
    else {
        uv_close((uv_handle_t*) client, NULL);
    }
}