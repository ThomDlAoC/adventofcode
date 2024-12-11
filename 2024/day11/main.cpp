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

std::pair<int, int> digitcount(uint64_t val)
{
    int count = 0;
    int mul10 = 1;
    while (val > 0)
    {
        val /= 10;
        count++;
        if(count%2==1)mul10 *= 10;
    }
    return {count, mul10};
}


uint64_t totalStonesAfterBlinks(std::unordered_map<uint64_t, uint64_t> stones, int blinks){
    for (int i = 0; i < blinks; ++i)
    {
        auto stones_cpy = stones;
        for (const auto [key, value] : stones_cpy)
        {
            stones[key]-=value;

            if (key == 0)
            {
                stones[1]+=value;
                continue;
            }

            auto [dc, mul10] = digitcount(key);
            if (dc % 2 == 0)
            {
                uint64_t left = key / mul10;
                uint64_t right = key % mul10;
                stones[left]+=value;
                stones[right]+=value;
                continue;
            }

            stones[key * 2024]+=value;
        }
    }

    uint64_t total{};

    for (const auto &[key, value] : stones)    
        total += value;
    

    return total;
}

std::string part1(std::stringstream &file_content)
{

    std::unordered_map<uint64_t, uint64_t> stones;
    uint64_t stone;
    while (file_content >> stone)    
        stones[stone]++;    

    return std::to_string(totalStonesAfterBlinks(stones, 25));
}

std::string part2(std::stringstream &file_content)
{

    std::unordered_map<uint64_t, uint64_t> stones;
    uint64_t stone;
    while (file_content >> stone)    
        stones[stone]++;    

    return std::to_string(totalStonesAfterBlinks(stones, 75));
}
