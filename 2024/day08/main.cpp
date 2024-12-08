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

struct node_t
{
    char id;
    int x;
    int y;
};

std::vector<std::string> parseInput(std::stringstream &file_content)
{
    std::vector<std::string> map;
    std::string line;
    while (std::getline(file_content, line))
    {
        map.push_back(line);
    }
    return map;
}

std::vector<node_t> getNodes(std::vector<std::string> &map)
{
    std::vector<node_t> res;
    size_t width = map[0].size();
    size_t height = map.size();

    for (size_t y = 0; y < height; y++)
    {
        for (size_t x = 0; x < width; x++)
        {
            if (map[y][x] != '.')
            {
                node_t node;
                node.id = map[y][x];
                node.x = x;
                node.y = y;
                res.push_back(node);
            }
        }
    }

    return res;
}

std::vector<std::pair<node_t, node_t>> getPairs(std::vector<node_t> &nodes)
{
    std::vector<std::pair<node_t, node_t>> res;
    for (size_t i = 0; i < nodes.size(); i++)
    {
        for (size_t j = i + 1; j < nodes.size(); j++)
        {
            if (nodes[i].id == nodes[j].id)
            {
                res.push_back(std::make_pair(nodes[i], nodes[j]));
            }
        }
    }
    return res;
}

std::string part1(std::stringstream &file_content)
{
    auto map = parseInput(file_content);
    auto nodes = getNodes(map);
    auto nodePairs = getPairs(nodes);

    std::unordered_set<int> antinodes;
    for (const auto &[node1, node2] : nodePairs)
    {
        int dx = node1.x - node2.x;
        int dy = node1.y - node2.y;

        for (int i = 1; i < 2; ++i)
        {
            int x = node1.x + dx * i;
            int y = node1.y + dy * i;
            if (x < 0 || x >= (int)map[0].size() || y < 0 || y >= (int)map.size())
                break;

            int key = x * map[0].size() + y;
            antinodes.insert(key);
        }

        for (int i = 1; i < 2; ++i)
        {
            int x = node2.x - dx * i;
            int y = node2.y - dy * i;
            if (x < 0 || x >= (int)map[0].size() || y < 0 || y >= (int)map.size())
                break;

            int key = x * map[0].size() + y;
            antinodes.insert(key);
        }
    }

    return std::to_string(antinodes.size());
}

std::string part2(std::stringstream &file_content)
{

    auto map = parseInput(file_content);
    auto nodes = getNodes(map);
    auto nodePairs = getPairs(nodes);

    std::unordered_set<int> antinodes;
    for (const auto &[node1, node2] : nodePairs)
    {
        int dx = node1.x - node2.x;
        int dy = node1.y - node2.y;

        for (int i = 0;; ++i)
        {
            int x = node1.x + dx * i;
            int y = node1.y + dy * i;
            if (x < 0 || x >= (int)map[0].size() || y < 0 || y >= (int)map.size())
                break;

            int key = x * map[0].size() + y;
            antinodes.insert(key);
        }

        for (int i = 0;; ++i)
        {
            int x = node2.x - dx * i;
            int y = node2.y - dy * i;
            if (x < 0 || x >= (int)map[0].size() || y < 0 || y >= (int)map.size())
                break;

            int key = x * map[0].size() + y;
            antinodes.insert(key);
        }
    }

    return std::to_string(antinodes.size());
}
