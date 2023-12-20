#ifndef NETWORK_H
#define NETWORK_H

#include <uv.h>

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
void on_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf);
void on_new_connection(uv_stream_t *server, int status);

#endif // NETWORK_H