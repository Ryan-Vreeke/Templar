#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <vector>

class tmpp {
public:
  std::string public_dir;
  std::map<std::string, std::string> block_contents;

  tmpp(std::string html_dir);
  tmpp(tmpp &&) = default;
  tmpp(const tmpp &) = default;
  tmpp &operator=(tmpp &&) = default;
  tmpp &operator=(const tmpp &) = default;
  ~tmpp();

  void prep_html(std::string &html);
  std::string load_file(std::string name);
  static bool isFile(std::string name);
  void add_file(const std::string &file);
  void remove_file(const std::string &file);
  std::string replace_var(std::string html, std::string var, std::string val);

  void iterate(std::string &html);

private:
  /*Returns false if failed to find def*/
  bool insert_block(std::string *html, std::string block);
  std::queue<int> find_end_pos(std::string text);
  std::queue<int> definitions(std::string text);
  std::vector<std::string> block_headers(std::string html);
  std::vector<int> find_all_var(std::string html, std::string var);

  /*Fill map with blocks defs as keys and content as values*/
  void fill_map(std::vector<std::string> &filePaths);
  void replace_headers(std::string *html);
  void listFiles(const std::filesystem::path &dirPath, std::vector<std::string> &filePaths);
  void remove_defs(std::string &html);

  /*
   * @breif: find position of all for loops in text. start positions returned
   * @return vector<int>: start positions of for loops in text
   */
  std::vector<int> find_for(std::string text);
  /*
   * @breif gets the number of iterations a loop needs to run for
   * @params html: string, for_pos: int
   * @return int: number of iterations loop runs
   * */
  int for_iterations(std::string html, int for_pos);

  /**
   * @return: the position of the end of the for loop
   */
  int get_for_content(std::string html, int for_pos, std::string &content);

  std::string block_key(std::string str);
};
