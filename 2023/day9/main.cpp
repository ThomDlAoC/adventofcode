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

std::vector<std::vector<int>> parseInput(std::stringstream &file_content)
{
    std::vector<std::vector<int>> result;
    std::string line;
    while (std::getline(file_content, line))
    {
        std::stringstream line_stream(line);
        std::vector<int> line_values;
        int value;
        while ((line_stream >> value))
            line_values.push_back(value);

        result.push_back(line_values);
    }
    return result;
}

int getNextLineValue(std::vector<int> &line)
{
    bool all_zero = false;
    std::vector<std::vector<int>> lines{line};
    while (!all_zero)
    {
        all_zero = true;
        auto &current_line = lines.back();
        std::vector<int> next_line;
        size_t len = current_line.size();
        for (size_t i = 1; i < len; i++)
        {
            int val = current_line[i] - current_line[i - 1];
            if (val != 0)            
                all_zero = false;
            
            next_line.push_back(val);
        }

        lines.push_back(next_line);
    }

    size_t len = lines.size();
    for (size_t i = len - 1; i > 0; i--)
    {
        auto &current_line = lines[i];
        auto &prev_line = lines[i - 1];
        prev_line.push_back(current_line.back() + prev_line.back());
    }

    return lines.front().back();
}

int getPrevLineValue(std::vector<int> &line)
{
    bool all_zero = false;
    std::reverse(line.begin(), line.end());
    std::vector<std::vector<int>> lines{line};
    while (!all_zero)
    {
        all_zero = true;
        auto &current_line = lines.back();
        std::vector<int> next_line;
        size_t len = current_line.size();
        for (size_t i = 1; i < len; i++)
        {
            int val = current_line[i - 1] - current_line[i];
            if (val != 0)
                all_zero = false;

            next_line.push_back(val);
        }

        lines.push_back(next_line);
    }


    size_t len = lines.size();

    for (size_t i = len - 1; i > 0; i--)
    {
        auto &current_line = lines[i];
        auto &prev_line = lines[i - 1];
        prev_line.push_back(prev_line.back() - current_line.back());
    }

    return lines.front().back();
}

std::string part1(std::stringstream &file_content)
{

    auto input = parseInput(file_content);
    int sum = 0;
    for (auto &line : input)
        sum += getNextLineValue(line);

    return std::to_string(sum);
}

std::string part2(std::stringstream &file_content)
{

    auto input = parseInput(file_content);
    int sum = 0;
    for (auto &line : input)
        sum += getPrevLineValue(line);

    return std::to_string(sum);
}