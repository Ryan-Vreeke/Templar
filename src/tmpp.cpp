#include "tmpp.h"
#include <algorithm>
#include <climits>
#include <cstddef>
#include <cstdio>
#include <queue>
#include <regex>
#include <vector>

namespace fs = std::filesystem;

tmpp::tmpp(std::string html_path) : public_dir(html_path)
{
  std::vector<std::string> filePaths;
  listFiles(html_path, filePaths);

  fill_map(filePaths);
}

tmpp::~tmpp() {}

void tmpp::listFiles(const fs::path &dirPath, std::vector<std::string> &filePaths)
{
  for (const auto &entry : fs::directory_iterator(dirPath))
  {
    if (entry.is_regular_file() && entry.path().extension() == ".html")
    {
      filePaths.push_back(entry.path().string());
    }
    else if (entry.is_directory())
    {
      listFiles(entry.path(), filePaths); // Recursively search directories
    }
  }
}

std::string tmpp::load_file(std::string filePath)
{
  std::stringstream buffer;
  std::ifstream file(filePath);

  if (!file.is_open())
  {
    std::cerr << "failed to open the file " << filePath << std::endl;
    return "";
  }

  buffer << file.rdbuf();
  file.close();

  return buffer.str();
}

std::string tmpp::def_content(std::string def, std::string html)
{
  int found = html.find(def);
  if (found == std::string::npos)
  {
    return "";
  }

  std::string sub_html = html.substr(found + def.length());

  // return html.substr(found + def.length(), find_next_end(sub_htm not sure if this is needed but remove find_next_end
  return ""; // TODO: not sure if this is needed but remove find_next_end
}

std::vector<std::string> tmpp::block_headers(std::string html)
{
  std::vector<std::string> headers;
  std::regex pattern("\\{\\{\\s*block\\s*\"([^\"]*)\"\\s*\\.\\s*\\}\\}");
  std::smatch matches;

  std::string::const_iterator searchStart(html.cbegin());

  while (std::regex_search(searchStart, html.cend(), matches, pattern))
  {
    headers.push_back(matches[0].str());
    searchStart = matches.suffix().first;
  }

  return headers;
}

bool tmpp::remove_def(std::string *str)
{
  int found = str->find("def");
  if (found == std::string::npos)
    return false;

  str->erase(found, 3);
  str->erase(remove(str->begin(), str->end(), ' '), str->end());
  return true;
}

std::string tmpp::block_key(std::string str)
{
  int found = str.find("\"");
  if (found == std::string::npos)
    return NULL;

  int end = str.find("\"", found + 1);
  return str.substr(found + 1, end - found - 1);
}

std::queue<int> definitions(std::string text)
{
  std::queue<int> pos;
  std::regex pattern("\\{\\{\\s*def\\s*block\\s*\"([^\"]*)\"\\s*\\.\\s*\\}\\}");

  auto words_begin = std::sregex_iterator(text.begin(), text.end(), pattern);
  auto words_end = std::sregex_iterator();

  for (std::sregex_iterator it = words_begin; it != words_end; ++it)
  {
    std::smatch match = *it;
    std::ptrdiff_t position = match.position(0); // position of the match in the string

    pos.push(position);
  }

  pos.push(INT_MAX);
  return pos;
}

std::queue<int> find_end_pos(std::string text)
{
  std::queue<int> pos;
  std::regex pattern("\\{\\{\\s*end\\s*\\}\\}");

  auto words_begin = std::sregex_iterator(text.begin(), text.end(), pattern);
  auto words_end = std::sregex_iterator();

  for (std::sregex_iterator it = words_begin; it != words_end; ++it)
  {
    std::smatch match = *it;
    std::ptrdiff_t position = match.position(0);

    pos.push(position);
  }

  pos.push(INT_MAX);
  return pos;
}

void tmpp::fill_map(std::vector<std::string> &filePaths)
{
  for (auto file : filePaths)
  {
    std::string html = load_file(file);

    std::queue<int> def_positions = definitions(html);
    std::queue<int> end_positions = find_end_pos(html);
    std::stack<int> def_stack;

    def_stack.push(def_positions.front());
    def_positions.pop();

    while (!def_stack.empty() || !def_positions.empty())
    {
      int d = def_positions.front();
      int e = end_positions.front();

      if (d < e)
      {
        def_stack.push(def_positions.front());
        def_positions.pop();
      }
      else if (d > e)
      {
        int start = def_stack.top();
        int end = end_positions.front();

        std::cout << html.substr(start, end - start) << std::endl;

        def_stack.pop();
        end_positions.pop();
      }
      else
      {
        break;
      }
    }

    // for (auto dpso : defs)
    // {
    //   std::string key = block_key(def);
    //   std::string content = def_content(def, html);

    //   block_contents[key] = content;
    // }
  }
}

void tmpp::replace_headers(std::string *html)
{
  std::vector<std::string> headers = block_headers(*html);

  for (auto header : headers)
  {
    insert_block(html, header);
  }
}

bool tmpp::insert_block(std::string *html, std::string block)
{
  int found = html->find(block);
  if (found == std::string::npos)
    return false;

  while (found != std::string::npos)
  {
    // block.erase(remove(block.begin(), block.end(), ' '), block.end());
    html->replace(found, block.length(), block_contents[block_key(block)]);
    found = html->find(block, found + 1);
  }

  return true;
}

std::string tmpp::prep_html(std::string path)
{
  std::string html = load_file(path);
  replace_headers(&html);

  return html;
}
