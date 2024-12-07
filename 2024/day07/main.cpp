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

using cal_test_t = std::pair<uint64_t, std::vector<uint64_t>>;

std::vector<cal_test_t> parseInput(std::stringstream &file_content)
{
    std::vector<cal_test_t> result;

    std::string line;
    while (std::getline(file_content, line))
    {
        uint64_t target;
        char tmp;
        std::stringstream ss(line);
        ss >> target >> tmp;
        std::vector<uint64_t> numbers_vector;
        uint64_t number;
        for (; ss >> number;)
        {
            numbers_vector.push_back(number);
        }
        result.push_back({target, numbers_vector});
    }

    return result;
}

bool canReachTarget(uint64_t target, std::vector<uint64_t> numbers, uint64_t index, uint64_t current)
{
    if (index == numbers.size())
    {
        return current == target;
    }

    if (current > target)
    {
        return false;
    }

    return canReachTarget(target, numbers, index + 1, current + numbers[index]) || canReachTarget(target, numbers, index + 1, current * numbers[index]);
}

std::string part1(std::stringstream &file_content)
{

    std::vector<cal_test_t> tests = parseInput(file_content);
    uint64_t result = 0;

    for (const auto &[target, numbers] : tests)
    {
        if (canReachTarget(target, numbers, 0, 0))
        {
            result += target;
        }
    }

    return std::to_string(result);
}

int poweroften(uint64_t number)
{
    int count = 1;
    while (number)
    {
        number /= 10;
        count *= 10;
    }
    return count;
}

bool canReachTargetCt(uint64_t target, std::vector<uint64_t> numbers, uint64_t index, uint64_t current)
{
    if (index == numbers.size())
    {
        return current == target;
    }

    if (current > target)
    {
        return false;
    }

    return canReachTargetCt(target, numbers, index + 1, current + numbers[index]) || canReachTargetCt(target, numbers, index + 1, current * poweroften(numbers[index]) + numbers[index]) || canReachTargetCt(target, numbers, index + 1, current * numbers[index]);
}

std::string part2(std::stringstream &file_content)
{

    std::vector<cal_test_t> tests = parseInput(file_content);
    uint64_t result = 0;

    for (const auto &[target, numbers] : tests)
    {
        if (canReachTargetCt(target, numbers, 0, 0))
        {
            result += target;
        }
    }

    return std::to_string(result);
}
