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

std::pair<std::vector<int>, std::vector<int>> parseFileContent(std::stringstream &file_content)
{
    std::vector<int> left;
    std::vector<int> right;
    for (std::string str; std::getline(file_content, str);)
    {
        int left_v, right_v;
        std::stringstream(str) >> left_v >> right_v;
        left.push_back(left_v);
        right.push_back(right_v);
    }
    return {left, right};
}

std::string part1(std::stringstream &file_content)
{
    auto [left, right] = parseFileContent(file_content);

    std::sort(left.begin(), left.end());
    std::sort(right.begin(), right.end());

    auto it_left = left.begin();
    auto it_right = right.begin();

    int sum_diff = 0;

    for (; it_left != left.end(); ++it_left, ++it_right)
        sum_diff += std::abs(*it_left - *it_right);

    return std::to_string(sum_diff);
}

std::string part2(std::stringstream &file_content)
{
    auto [left, right] = parseFileContent(file_content);

    std::unordered_map<int, int> count_map;
    for (const auto v : right)
        count_map[v]++;

    int sum_count = 0;
    for (const auto v : left)
        sum_count += v * count_map[v];

    return std::to_string(sum_count);
}
