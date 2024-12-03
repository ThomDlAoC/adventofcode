#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <fstream>
#include <sstream>
#include <chrono>
#include <functional>
#include <memory>
#include <numeric>
#include <cstring>
#include <climits>
#include <cmath>
#include <regex>

/*************
 * Setup code
 **************/

std::string part1(std::stringstream &file_content);
std::string part2(std::stringstream &file_content);

std::optional<std::stringstream> readFileContent(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Could not open file " << path << std::endl;
        return std::nullopt;
    }

    std::stringstream file_content;
    file_content << file.rdbuf();
    file.close();

    return std::move(file_content);
};

void executeFunction(std::stringstream &file_content, std::function<std::string(std::stringstream &)> function, std::string label, std::string expected)
{

    file_content.clear();
    file_content.seekg(0, std::ios::beg);
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::string result = function(file_content);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    if (result != expected)
    {
        std::cerr << "\033[1;31m" << label << " failed. Expected " << expected << " but got " << result << "\033[0m" << std::endl;
    }
    else
    {
        std::cout << "\033[1;32m" << label << " result: " << result << "\033[0m" << std::endl;
    }
    std::cerr << "Execution time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
}

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <path> <expected_path>" << std::endl;
        return 1;
    }

    std::string path = argv[1];
    std::string expected_path = argv[2];
    std::optional<std::stringstream> file_content = readFileContent(path);
    std::optional<std::stringstream> file_expected_content = readFileContent(expected_path);

    if (!file_content)
    {
        std::cerr << "Could not read file " << path << std::endl;
        return 1;
    }

    if (!file_expected_content)
    {
        std::cerr << "Could not read file " << expected_path << std::endl;
        return 1;
    }

    std::stringstream &file_content_ref = *file_content;
    std::stringstream &file_expected_content_ref = *file_expected_content;

    std::string expected;

    file_expected_content_ref >> expected;
    executeFunction(file_content_ref, part1, "Part 1", expected);
    file_expected_content_ref >> expected;
    executeFunction(file_content_ref, part2, "Part 2", expected);

    return 0;
}

/***************************************
 *              Solutions
 **************************************/

enum class ParseState
{
    NONE = -1,
    NUM_LEFT = 0,
    NUM_RIGHT = 1,
    MULT = 2,
    DO = 3,
    DONT = 4
};

struct Trie
{
    std::unordered_map<char, Trie> children;
    ParseState val;
};

Trie makeTrie()
{

    std::vector<std::vector<std::pair<char, ParseState>>> tr = {{{'m', ParseState::NONE}, {'u', ParseState::NONE}, {'l', ParseState::NONE}, {'(', ParseState::NUM_LEFT}, {',', ParseState::NUM_RIGHT}, {')', ParseState::MULT}},
                                                                {{'d', ParseState::NONE}, {'o', ParseState::NONE}, {'(', ParseState::NONE}, {')', ParseState::DO}},
                                                                {{'d', ParseState::NONE}, {'o', ParseState::NONE}, {'n', ParseState::NONE}, {'\'', ParseState::NONE}, {'t', ParseState::NONE}, {'(', ParseState::NONE}, {')', ParseState::DONT}}};
    Trie root{};

    for (auto &v : tr)
    {
        Trie *current = &root;
        for (auto &[c, val] : v)
        {
            if (current->children.find(c) == current->children.end())
                current->children[c] = Trie{};
            current = &current->children[c];
            current->val = val;
        }
    }

    return root;
}

std::string part1(std::stringstream &file_content)
{
    std::string str = file_content.str();
    int result{};

    Trie root = makeTrie();

    int left{};
    int right{};

    Trie *current = &root;
    for (char c : str)
    {

        if (current->val == ParseState::NUM_LEFT && std::isdigit(c))
        {
            left = left * 10 + (c - '0');
            continue;
        }
        else if (current->val == ParseState::NUM_RIGHT && std::isdigit(c))
        {
            right = right * 10 + (c - '0');
            continue;
        }
        else if (current->children.count(c))
        {
            current = &current->children[c];
            continue;
        }

        if (current->val == ParseState::MULT)
            result += left * right;

        current = &root, left = 0, right = 0;
        if (current->children.count(c))
            current = &current->children[c];
    }

    return std::to_string(result);
}

std::string part2(std::stringstream &file_content)
{

    std::string str = file_content.str();
    int result{};

    Trie root = makeTrie();

    bool pdo = true;
    int left{};
    int right{};

    Trie *current = &root;
    for (char c : str)
    {

        if (current->val == ParseState::NUM_LEFT && std::isdigit(c))
        {
            left = left * 10 + (c - '0');
            continue;
        }
        else if (current->val == ParseState::NUM_RIGHT && std::isdigit(c))
        {
            right = right * 10 + (c - '0');
            continue;
        }
        else if (current->children.count(c))
        {
            current = &current->children[c];
            continue;
        }

        if (current->val == ParseState::MULT && pdo)
        {
            result += left * right;
            current = &root, left = 0, right = 0;
        }
        else if (current->val == ParseState::DO)
            pdo = true;
        else if (current->val == ParseState::DONT)
            pdo = false;

        current = &root, left = 0, right = 0;
        if (current->children.count(c))
            current = &current->children[c];
    }

    return std::to_string(result);
}
