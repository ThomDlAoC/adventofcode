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


std::vector<int> parseLine(const std::string &line)
{
    std::vector<int> numbers;
    std::stringstream ss(line);
    for (int number; ss >> number;)
        numbers.push_back(number);

    return numbers;
}

bool isSafeReport(const std::vector<int> &numbers, int skip = -1)
{
    int diff = 0;
    int sz = numbers.size();
    int prev = skip == 0 ? numbers[1] : numbers[0];
    for (int i = skip == 0 ? 2 : 1; i < sz; i++)
    {
        if (skip == i)
            continue;

        int next = numbers[i];
        int d = next - prev;
        prev = next;

        if (d == 0)
            return false;

        if ((d > 0 && diff < 0) || (d < 0 && diff > 0))
            return false;

        if (std::abs(d) > 3)
            return false;

        diff = d;
    }

    return true;
}


std::string part1(std::stringstream &file_content)
{
    int safe_reports{};

    for (std::string line; std::getline(file_content, line);)
    {
        std::vector<int> numbers = parseLine(line);
        safe_reports += isSafeReport(numbers, -1);
    }

    return std::to_string(safe_reports);
}

std::string part2(std::stringstream &file_content)
{
    int safe_reports{};

    for (std::string line; std::getline(file_content, line);)
    {
        std::vector<int> numbers = parseLine(line);
        int sz = numbers.size();
        for (int i = -1; i < sz; ++i)
        {
            if (isSafeReport(numbers, i))
            {
                safe_reports++;
                break;
            }
        }
    }

    return std::to_string(safe_reports);
}