#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <vector>

class tmpp
{
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
  void add_file(const std::string& file);
  void remove_file(const std::string& file);

 private:
	/*Returns false if failed to find def*/
	bool insert_block(std::string *html, std::string block);
	std::queue<int> find_end_pos(std::string text);
	std::queue<int> definitions(std::string text);

	std::vector<std::string> block_headers(std::string html);

	/*Fill map with blocks defs as keys and content as values*/
	void fill_map(std::vector<std::string> &filePaths);
	void replace_headers(std::string *html);
	void listFiles(const std::filesystem::path &dirPath, std::vector<std::string> &filePaths);

	std::string block_key(std::string str);
};
