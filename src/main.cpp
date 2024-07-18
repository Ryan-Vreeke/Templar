#include "tmpp.h"
#include "webserve.h"
#include <csignal>
#include <cstdio>
#include <cstring>
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

static std::vector<int> regex_pos(std::string text) {
  std::vector<int> poss;
  std::regex pattern(R"(\{\{end\}\})"); 

  auto words_begin = std::sregex_iterator(text.begin(), text.end(), pattern);
  auto words_end = std::sregex_iterator();

  std::cout << "Found matches at positions: ";
  for (std::sregex_iterator it = words_begin; it != words_end; ++it) {
    std::smatch match = *it;
    std::ptrdiff_t position = match.position(0); // position of the match in the string
    std::cout << position << " ";
    poss.push_back(position);
  }
  std::cout << std::endl;

  return poss;
}

int main(int argc, char *argv[]) {
  // std::signal(SIGINT, signalHandler);

  std::string text = "{{def block \"header\" .}}<!doctype html> <html lang=\"en\"> <head> <meta charset=\"UTF-8\" /> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" /> <title></title> <link href=\"css/style.css\" rel=\"stylesheet\" /> </head> <body> {{ def block \"index\" .}} <h1>Hello</h1> {{end}} {{end}} {{block \"swap\" .}} </body> </html> {{ def block \"swap\" . }} <h5>new hello</h5> {{end}}";
  auto p = regex_pos(text);

  for(auto i : p){
    std::cout << text.substr(i, 7) << std::endl;
  }

  // web->start();
  return 0;
}
