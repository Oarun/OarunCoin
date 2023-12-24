#ifndef NETWORK_H
#define NETWORK_H

#include <uv.h>

void onConnectGetPeers(uv_connect_t *req, int status);
void onWriteGetPeers(uv_write_t *req, int status);
void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
#endif // NETWORK_H