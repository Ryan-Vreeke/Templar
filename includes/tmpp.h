#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

class tmpp {
public:
  tmpp(std::string html_dir);
  tmpp(tmpp &&) = default;
  tmpp(const tmpp &) = default;
  tmpp &operator=(tmpp &&) = default;
  tmpp &operator=(const tmpp &) = default;
  ~tmpp();

  std::string public_dir;
  std::map<std::string, std::string> block_contents;

  std::string prep_html(std::string path);
  std::string load_file(std::string name);

private:
  /*Returns false if failed to find def*/
  bool remove_def(std::string *str);
  bool insert_block(std::string *html, std::string block);

  std::vector<std::string> block_headers(std::string html);
  std::string def_content(std::string def, std::string html);

  void listFiles(const std::filesystem::path &dirPath, std::vector<std::string> &filePaths);
  /*Fill map with blocks defs as keys and content as values*/
  void fill_map(std::vector<std::string> &filePaths);
  void replace_headers(std::string *html);

  std::string block_key(std::string str);
};
