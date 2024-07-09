#pragma once
#include <vector>
#include <string>

// enum RequestTypes {
//   GET,
//   POST,
//   HEAD,
//   PUT,
//   DELETE,
//   CONNECT,
// };

class recv_cb_t {
public:
  std::string type;
  int client_fd;
  std::vector<std::string> headers;
  std::string path;
  std::string protocal;

  recv_cb_t();
  recv_cb_t(recv_cb_t &&) = default;
  recv_cb_t(const recv_cb_t &) = default;
  recv_cb_t &operator=(recv_cb_t &&) = default;
  recv_cb_t &operator=(const recv_cb_t &) = default;
  ~recv_cb_t();

private:
};
