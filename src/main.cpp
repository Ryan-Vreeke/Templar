#include "tmpp.h"
#include "webserve.h"
#include <cstdio>
#include <cstring>
#include <string>
#include <stdlib.h>
#include <sys/socket.h>

tmpp tm{"./public"};

void request_recv(recv_cb_t cb) {
  std::string html = tm.prep_html("./public/index.html");
  printf("\npath: %s\n", cb.path.c_str());

}

int main(int argc, char *argv[]) {

  // for (const auto &pair : tm.block_contents) {
  //   // std::cout << "Key: " << pair.first << ", Value: " << pair.second <<
  //   // std::endl;
  // }

  webserve web{&request_recv, 8080};

  web.start();
  return 0;
}
