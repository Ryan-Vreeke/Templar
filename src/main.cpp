#include <csignal>
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

#include "WebContext.h"
#include "webserve.h"

webserve *web;
using namespace std;

void signalHandler(int signal) {
  if (signal == SIGINT) {
    std::cout << "Closing Server" << std::endl;
    web->stop();

    delete web;
  }
}

using json = nlohmann::json;

int main(int argc, char *argv[]) {

  json ex1 = json::parse(R"(
    {
      "pi": 3.141,
      "happy": true
    }
  )");

  for (json::iterator it = ex1.begin(); it != ex1.end(); it++) {
    cout << it.key() << ":" << it.value() << endl;
  }

  return 1;
  /*int port;*/
  /*cin >> port;*/
  /**/
  /*web = new webserve{"./public", port};*/
  /**/
  /*web->GET("/", [](WebContext ctx) -> std::string {*/
  /*  json ex1 = json::parse(R"(*/
  /*    {*/
  /*      "pi": 3.141,*/
  /*      "happy": true*/
  /*    }*/
  /*  )");*/
  /**/
  /*  return ctx.Render(200, "index", ex1);*/
  /*});*/
  /**/
  /*web->start();*/
  return 0;
}
