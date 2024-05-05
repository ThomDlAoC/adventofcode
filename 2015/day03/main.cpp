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
#include <unordered_set>

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


std::string part1(std::stringstream &file_content)
{
    std::string line;
    std::getline(file_content, line);
    int px = 0, py = 0;
    auto pair_hash = [](const std::pair<int, int> &p) { return p.first ^ p.second; };
    auto pair_cmp = [](const std::pair<int, int> &p1, const std::pair<int, int> &p2) { return p1.first == p2.first && p1.second == p2.second; };    
    std::unordered_set<std::pair<int, int>, decltype(pair_hash), decltype(pair_cmp)> visited(0, pair_hash, pair_cmp);
    visited.insert({px, py});
    for(const char &c : line)
    {
        switch(c)
        {
            case '^':
                py++;
                break;
            case 'v':
                py--;
                break;
            case '>':
                px++;
                break;
            case '<':
                px--;
                break;
        }
        visited.insert({px, py});
    }

    return std::to_string(visited.size());
}

std::string part2(std::stringstream &file_content)
{

    std::string line;
    std::getline(file_content, line);
    std::pair<int,int> santa{0,0}, robot{0,0};
    auto pair_hash = [](const std::pair<int, int> &p) { return p.first ^ p.second; };
    auto pair_cmp = [](const std::pair<int, int> &p1, const std::pair<int, int> &p2) { return p1.first == p2.first && p1.second == p2.second; };    

    std::unordered_set<std::pair<int, int>, decltype(pair_hash), decltype(pair_cmp)> visited(0, pair_hash, pair_cmp);
    visited.insert({0, 0});
    bool turn = false;

    for(const char &c : line)
    {
        std::pair<int,int> &current = turn ? robot : santa;
        turn = !turn;
        switch(c)
        {
            case '^':
                current.second++;
                break;
            case 'v':
                current.second--;
                break;
            case '>':
                current.first++;
                break;
            case '<':
                current.first--;
                break;
        }

        visited.insert(current);
    }    

    return std::to_string(visited.size());
}
