#include "tmpp.h"
#include <string>

int main(int argc, char *argv[]) {
  tmpp tm{"./public"};

  for (const auto &pair : tm.block_contents) {
    // std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
  }

  std::string html = tm.prep_html("./public/index.html");

  std::cout << html << std::endl;
  return 0;
}
