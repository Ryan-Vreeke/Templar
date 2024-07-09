#include "tmpp.h"
#include "webserve.h"
#include <cstring>
#include <string>
#include <stdlib.h>
#include <sys/socket.h>

void request_recv(recv_cb_t cb) {
  std::cout << "here" << std::endl;

  send(cb.client_fd, "hello", strlen("hello"), 0);
}

int main(int argc, char *argv[]) {
  tmpp tm{"./public"};

  for (const auto &pair : tm.block_contents) {
    // std::cout << "Key: " << pair.first << ", Value: " << pair.second <<
    // std::endl;
  }

  std::string html = tm.prep_html("./public/index.html");

  std::cout << html << std::endl;

  webserve web{&request_recv, 8080};

  web.start();
  return 0;
}
