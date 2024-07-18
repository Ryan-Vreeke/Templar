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

static void def_pos(std::string text, std::queue<int> &pos, std::queue<std::string> &defs) {
  std::regex pattern("\\{\\{\\s*def\\s*block\\s*\"([^\"]*)\"\\s*\\.\\s*\\}\\}");

  auto words_begin = std::sregex_iterator(text.begin(), text.end(), pattern);
  auto words_end = std::sregex_iterator();

  for (std::sregex_iterator it = words_begin; it != words_end; ++it) {
    std::smatch match = *it;
    std::ptrdiff_t position = match.position(0); // position of the match in the string

    pos.push(position + match.length());
    defs.push(match[0]);
  }
}

static void end_pos(std::string text,std::queue<int> &pos, std::queue<std::string> &ends) {
  std::regex pattern("\\{\\{\\s*end\\s*\\}\\}");

  auto words_begin = std::sregex_iterator(text.begin(), text.end(), pattern);
  auto words_end = std::sregex_iterator();

  for (std::sregex_iterator it = words_begin; it != words_end; ++it) {
    std::smatch match = *it;
    std::ptrdiff_t position = match.position(0);

    pos.push(position);
    ends.push(match[0]);
  }

}

int main(int argc, char *argv[]) {
  // std::signal(SIGINT, signalHandler);
  
  tmpp t{"./public/"};
  std::string text = t.load_file("./public/index.html");
  std::cout << text << std::endl;

  std::queue<int> endPos;
  std::queue<int> defPos;

  std::queue<std::string> defs;
  std::queue<std::string> ends;

  end_pos(text, endPos, ends);
  def_pos(text, defPos, defs);

  std::cout << text.substr(defPos.front(), endPos.front() - defPos.front()) << std::endl;
  std::cout << defs.front() << std::endl;
  // web->start();
  return 0;
}
