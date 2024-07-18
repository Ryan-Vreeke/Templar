#include "tmpp.h"
#include "webserve.h"
#include <csignal>
#include <cstdio>
#include <cstring>
#include <ostream>
#include <queue>
#include <regex>
#include <stdlib.h>
#include <string>
#include <sys/socket.h>

// webserve *web = new webserve{"./public/", 8000};

// void signalHandler(int signal) {
//   if (signal == SIGINT) {
//     std::cout << "Closing Server" << std::endl;

//     delete web;
//   }
// }


int main(int argc, char *argv[]) {
  // std::signal(SIGINT, signalHandler);
  
  tmpp t{"./public/"};

  // web->start();
  return 0;
}
