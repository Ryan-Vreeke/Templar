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

  int port;
  cin >> port;

  web = new webserve{"../public", port};

  web->GET("/api", [](WebContext ctx) -> std::string {
    json ex1 = json::parse(R"(
      {
        "pi": 3.141,
        "happy": true
      }
    )");

    return ctx.Render(200, ex1);
  });

  web->GET("/", [](WebContext ctx) -> std::string {
    json ex1 = json::parse(R"(
      {
        "pi": 3.141,
        "happy": true
      }
    )");

    return ctx.Render(200, "index", ex1);
  });

  web->start();
  return 0;
}
