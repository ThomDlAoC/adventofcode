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
#include <iomanip>

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
    std::vector<int> seq_len;
    std::vector<int> nums(256);
    std::iota(nums.begin(), nums.end(), 0);

    for (std::string line; std::getline(file_content, line, ',');)
        seq_len.push_back(std::stoi(line));

    int skip_size = 0;
    int current_position = 0;

    for (int len : seq_len)
    {
        std::vector<int> range_num(len, 0);
        for (int j = 0; j < len; j++)
            range_num[j] = nums[(current_position + j) % nums.size()];
        std::reverse(range_num.begin(), range_num.end());
        for (int j = 0; j < len; j++)
            nums[(current_position + j) % nums.size()] = range_num[j];
        current_position += len + skip_size;
        skip_size++;
    }

    return std::to_string(nums[0] * nums[1]);
}

std::string part2(std::stringstream &file_content)
{
    std::vector<int> seq_len;
    std::vector<int> nums(256);
    std::iota(nums.begin(), nums.end(), 0);

    std::string line;
    file_content >> line;
    for (char c : line)
        seq_len.push_back(c);

    std::vector<int> suffix = {17, 31, 73, 47, 23};
    seq_len.insert(seq_len.end(), suffix.begin(), suffix.end());

    int skip_size = 0;
    int current_position = 0;

    for (int i = 64; i--;)
    {
        for (int len : seq_len)
        {
            std::vector<int> range_num(len, 0);
            for (int j = 0; j < len; j++)
                range_num[j] = nums[(current_position + j) % nums.size()];
            std::reverse(range_num.begin(), range_num.end());
            for (int j = 0; j < len; j++)
                nums[(current_position + j) % nums.size()] = range_num[j];
            current_position += len + skip_size;
            skip_size++;
        }
    }

    std::vector<int> final_hash(16, 0);
    for (int i = 0; i < 16; i++)
        for (int j = 0; j < 16; j++)
            final_hash[i] ^= nums[i * 16 + j];

    std::stringstream ss;
    for (int i : final_hash)
        ss << std::setfill('0') << std::setw(2) << std::hex << i;

    return ss.str();
}
