#include "tmpp.h"

#include <algorithm>
#include <cctype>
#include <climits>
#include <cstddef>
#include <cstdio>
#include <filesystem>
#include <format>
#include <iterator>
#include <queue>
#include <regex>
#include <string>
#include <vector>

namespace fs = std::filesystem;

tmpp::tmpp(std::string html_path) : public_dir(html_path) { init_temple(); }

tmpp::~tmpp() {}

void tmpp::reload_defs() {
  block_contents.clear();
  init_temple();
}

void tmpp::init_temple() {
  std::vector<std::string> filePaths;
  listFiles(public_dir, filePaths);

  fill_map(filePaths);
}

void tmpp::remove_defs(std::string &html) {
  std::queue defs = definitions(html);
  std::queue ends = find_end_pos(html);

  while (defs.front() != INT_MAX) {
    int start = defs.front();
    int end = ends.front();

    int bloc = html.find("}}", end) + 2;
    html.erase(start, bloc - start);

    defs.pop();
    ends.pop();
  }
}

std::vector<int> tmpp::search_regex(const std::string &html,
                                    const std::string &regex) {
  std::regex pattern(regex);
  std::vector<int> pos;

  auto words_begin = std::sregex_iterator(html.begin(), html.end(), pattern);
  auto words_end = std::sregex_iterator();

  for (std::sregex_iterator it = words_begin; it != words_end; ++it) {
    std::smatch match = *it;
    std::ptrdiff_t position =
        match.position(0); // position of the match in the string

    pos.push_back(position);
  }

  return pos;
}

std::vector<int> tmpp::find_all_var(const std::string &html,
                                    const std::string &var) {
  std::string search = std::format("\\.{}\\b", var);
  return search_regex(html, search);
}

void tmpp::replace_var(std::string &html, std::string var, std::string val) {
  std::vector<int> var_pos = find_all_var(html, var);
  int sub_val = 0;

  for (int i = 0; i < var_pos.size(); i++) {
    int start = var_pos[i];
    int len = var.length() + 1;

    html.erase(start, len);
    html.insert(start, val);

    if (i != var_pos.size()) {
      sub_val += (len - val.size());
      var_pos[i + 1] -= sub_val;
    }
  }
}

void tmpp::prep_html(std::string &html) {
  remove_defs(html);
  replace_headers(&html);
}

bool tmpp::isFile(std::string path) { return fs::exists(path); }

void tmpp::listFiles(const fs::path &dirPath,
                     std::vector<std::string> &filePaths) {
  for (const auto &entry : fs::directory_iterator(dirPath)) {
    if (entry.is_regular_file() && entry.path().extension() == ".html") {
      filePaths.push_back(entry.path().string());
    } else if (entry.is_directory()) {
      listFiles(entry.path(), filePaths); // Recursively search directories
    }
  }
}

std::string tmpp::load_file(std::string filePath) {
  std::stringstream buffer;
  std::ifstream file(filePath);

  if (!file.is_open()) {
    std::cerr << "failed to open the file " << filePath << std::endl;
    return "";
  }

  buffer << file.rdbuf();
  file.close();

  return buffer.str();
}

std::vector<std::string> tmpp::block_headers(std::string html) {
  std::vector<std::string> headers;
  std::regex pattern("\\{\\{\\s*block\\s*\"([^\"]*)\"\\s*\\.\\s*\\}\\}");
  std::smatch matches;

  std::string::const_iterator searchStart(html.cbegin());

  while (std::regex_search(searchStart, html.cend(), matches, pattern)) {
    headers.push_back(matches[0].str());
    searchStart = matches.suffix().first;
  }

  return headers;
}

std::vector<int> tmpp::find_break(const std::string &text) {
  std::string pattern("\\{\\s*\\{/for\\s*\\}\\s*\\}");
  return search_regex(text, pattern);
}

std::vector<int> tmpp::find_for(const std::string &text) {
  std::string pattern("\\{\\s*\\{for\\s*\\.\\(\\d+\\)\\s*\\}\\s*\\}");
  return search_regex(text, pattern);
}

void tmpp::replace_for(std::string &html) {
  std::vector<int> for_positions = find_for(html);
  std::vector<int> break_positions = find_break(html);
  std::stack<int> fStack;

  for_positions.push_back(INT_MAX);
  break_positions.push_back(INT_MAX);

  int startOffset = 0;
  int endOffset = 0;

  int i = 0;
  int j = 0;
  while (1) {
    int element = for_positions[j];
    if (break_positions[i] == element) {
      break;
    }

    if (element < break_positions[i]) {
      fStack.push(element);
      j++;
      continue;
    }

    int start = fStack.top() + startOffset;
    int end = break_positions[i] + endOffset;
    fStack.pop();
    std::string content = get_content(html, start, end);

    end = html.find("}}", end) + 2;

    int iter = get_iterations(html.substr(start, end - start));
    html.erase(start, end - start);

    for (int k = 0; k < iter; k++) {
      html.insert(start, content);
      start += content.length();
    }

    if (fStack.empty()) {
      startOffset += start - end;
      endOffset += start - end;
    } else {
      endOffset += start - end;
    }

    i++;
  }
}

// TODO: TEST CODE
int tmpp::get_iterations(const std::string &sub_html) {
  bool found_digit = false;
  std::string number;
  for (char c : sub_html) {
    if (!std::isdigit(c)) {
      if (found_digit) {
        break;
      }

      continue;
    }

    number += c;
    found_digit = true;
  }

  return std::stoi(number);
}

std::string tmpp::get_content(const std::string &html, int start, int end) {
  start = html.find("}}", start) + 2;
  return html.substr(start, end - start);
}

std::queue<int> tmpp::definitions(const std::string &text) {
  std::string search =
      "\\{\\{\\s*def\\s*block\\s*\"([^\"]*)\"\\s*\\.\\s*\\}\\}";
  std::queue<int> posQ;
  std::vector<int> def_pos = search_regex(text, search);

  for (const auto &pos : def_pos) {
    posQ.push(pos);
  }

  posQ.push(INT_MAX);
  return posQ;
}

std::queue<int> tmpp::find_end_pos(const std::string &text) {
  std::queue<int> pos;
  std::string pattern("\\{\\{\\s*end\\s*\\}\\}");
  std::vector<int> endPos = search_regex(text, pattern);

  for (const auto &end : endPos) {
    pos.push(end);
  }

  pos.push(INT_MAX);
  return pos;
}

void tmpp::fill_map(std::vector<std::string> &filePaths) {
  for (auto file : filePaths) {
    add_file(file);
  }
}

void tmpp::replace_headers(std::string *html) {
  std::vector<std::string> headers = block_headers(*html);

  for (auto header : headers) {
    insert_block(html, header);
  }
}

bool tmpp::insert_block(std::string *html, std::string block) {
  int found = html->find(block);
  if (found == std::string::npos)
    return false;

  while (found != std::string::npos) {
    html->replace(found, block.length(), block_contents[block_key(block)]);
    found = html->find(block, found + 1);
  }

  return true;
}

std::string tmpp::block_key(std::string str) {
  int found = str.find("\"");
  if (found == std::string::npos)
    return NULL;

  int end = str.find("\"", found + 1);
  return str.substr(found + 1, end - found - 1);
}

void tmpp::add_file(const std::string &file) {
  std::string html = load_file(file);
  if (html.empty())
    return;

  std::queue<int> def_positions = definitions(html);
  std::queue<int> end_positions = find_end_pos(html);
  std::stack<int> def_stack;

  if (def_positions.front() == INT_MAX || end_positions.front() == INT_MAX)
    return;

  def_stack.push(def_positions.front());
  def_positions.pop();

  while (!def_stack.empty() || !def_positions.empty()) {
    int d = def_positions.front();
    int e = end_positions.front();

    if (d < e) {
      def_stack.push(def_positions.front());
      def_positions.pop();
    } else if (d > e) {
      int start = def_stack.top();
      int end = end_positions.front();
      int len = html.find("}}", start) + 2;

      std::string key = block_key(html.substr(start, len));
      std::string content = html.substr(len, end - len);

      if (block_contents.contains(key)) {
        block_contents.erase(key);
      }

      block_contents[key] = content;

      def_stack.pop();
      end_positions.pop();
    } else {
      break;
    }
  }
}

void tmpp::remove_file(const std::string &file) {
  std::string html = load_file(file);
  std::queue<int> def_positions = definitions(html);

  while (def_positions.front() != INT_MAX) {
    int start = def_positions.front();
    int len = html.find("}}", start) + 2;

    std::string key = block_key(html.substr(start, len));

    block_contents.erase(key);
    def_positions.pop();
  }
}
