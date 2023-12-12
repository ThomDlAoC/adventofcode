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
#include <queue>

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

unsigned long long getArrangements(std::string &line, std::vector<int> &container, int lineidx, int containerIdx, int currentCount, std::unordered_map<int, unsigned long long> &cache)
{
    int cache_key = lineidx * 10000 + containerIdx * 100 + currentCount;
    if (cache.find(cache_key) != cache.end())
        return cache[cache_key];

    if (lineidx == (int)line.size())
    {
        if (containerIdx == (int)container.size() - 1 && currentCount == container[containerIdx])
            return 1;
        else if (containerIdx == (int)container.size() && currentCount == 0)
            return 1;

        return 0;
    }

    if (currentCount && containerIdx == (int)container.size())
        return 0;

    if (currentCount && currentCount > container[containerIdx])
        return 0;

    unsigned long long res = 0;
    if (line[lineidx] == '.')
    {
        if (currentCount && currentCount < container[containerIdx])
            return 0;
        res = getArrangements(line, container, lineidx + 1, currentCount ? containerIdx + 1 : containerIdx, 0, cache);
    }

    if (line[lineidx] == '#')
    {
        res = getArrangements(line, container, lineidx + 1, containerIdx, currentCount + 1, cache);
    }

    if (line[lineidx] == '?')
    {
        unsigned long long sum = 0;
        if (currentCount && currentCount < container[containerIdx])
            sum = 0;
        else
            sum = getArrangements(line, container, lineidx + 1, currentCount ? containerIdx + 1 : containerIdx, 0, cache);
        sum += getArrangements(line, container, lineidx + 1, containerIdx, currentCount + 1, cache);
        res = sum;
    }

    return cache[cache_key] = res;
}

std::vector<std::pair<std::string, std::vector<int>>> parseInput(std::stringstream &file_content)
{
    std::vector<std::pair<std::string, std::vector<int>>> lines;

    std::string line;
    while (std::getline(file_content, line))
    {
        std::stringstream line_stream(line);
        std::string line_str;
        std::vector<int> container;
        line_stream >> line_str;

        for (std::string container_str; std::getline(line_stream, container_str, ',');)
        {
            container.push_back(std::stoi(container_str));
        }

        lines.push_back(std::make_pair(line_str, container));
    }

    return lines;
}

std::string part1(std::stringstream &file_content)
{

    auto lines = parseInput(file_content);
    unsigned long long sum = 0;
    for (auto &line : lines)
    {
        std::unordered_map<int, unsigned long long> cache;
        int res = getArrangements(line.first, line.second, 0, 0, 0, cache);
        sum += res;
    }

    return std::to_string(sum);
}

std::string part2(std::stringstream &file_content)
{

    auto lines = parseInput(file_content);

    unsigned long long sum = 0;
    for (auto &line : lines)
    {

        std::unordered_map<int, unsigned long long> cache;

        line.first = line.first + "?" + line.first + "?" + line.first + "?" + line.first + "?" + line.first;
        auto initial_container = line.second;
        for (int i = 0; i < 4; i++)
            line.second.insert(line.second.end(), initial_container.begin(), initial_container.end());

        sum += getArrangements(line.first, line.second, 0, 0, 0, cache);
    }

    return std::to_string(sum);
}
