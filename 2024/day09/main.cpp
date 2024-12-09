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

std::string part1(std::stringstream &file_content)
{

    std::string full_str = file_content.str();

    int total_blocks = 0;
    for (const char &c : full_str)    
        total_blocks += c - '0';
    

    std::vector<int> blocks(total_blocks, 0);

    int block_index = 0;
    int block_value = 0;
    bool free_blk = false;
    for (const char &c : full_str)
    {
        int blk_cnt = c - '0';
        for (int j = 0; j < blk_cnt; j++)        
            blocks[block_index++] = free_blk ? -1 : block_value;
        
        free_blk = !free_blk;
        if (free_blk)        
            block_value++;
        
    }

    for (int i = 0, j = blocks.size() - 1; i < j; ++i)
    {
        if (blocks[i] == -1)
        {
            while (blocks[j] == -1)            
                j--;            
            std::swap(blocks[i], blocks[j--]);            
        }
    }

    uint64_t sum = 0;
    for (int i = 0; i < static_cast<int>(blocks.size()); i++)
    {
        if (blocks[i] == -1)
            break;
        sum += i * blocks[i];
    }

    return std::to_string(sum);
}

std::string part2(std::stringstream &file_content)
{

    std::string full_str = file_content.str();

    int total_blocks = 0;
    for (const char &c : full_str)
    {
        total_blocks += c - '0';
    }

    std::vector<int> blocks(total_blocks, 0);

    int block_index = 0;
    int block_value = 0;
    bool free_blk = false;
    std::vector<std::tuple<int, int, int>> used_blocks;
    std::vector<std::tuple<int, int, int>> free_blocks;

    for (const char &c : full_str)
    {
        int blk_cnt = c - '0';
        int start_index = block_index;
        int end_index = block_index + blk_cnt - 1;
        for (int j = 0; j < blk_cnt; j++)
            blocks[block_index++] = free_blk ? -1 : block_value;

        if (free_blk)
            free_blocks.push_back({start_index, end_index, -1});
        else
            used_blocks.push_back({start_index, end_index, block_value});

        free_blk = !free_blk;
        if (free_blk)
            block_value++;
    }

    for (auto ub_it = used_blocks.rbegin(); ub_it != used_blocks.rend(); ++ub_it)
    {
        const auto &[start_index, end_index, value] = *ub_it;
        for (auto &[f_start_index, f_end_index, f_value] : free_blocks)
        {
            if (start_index < f_start_index)
                break;

            int f_sz = f_end_index - f_start_index + 1;
            int ub_sz = end_index - start_index + 1;
            if (f_sz < ub_sz)
                continue;

            for (int i = 0; i < ub_sz; i++)
                std::swap(blocks[f_start_index++], blocks[start_index + i]);

            break;
        }
    }

    uint64_t sum = 0;

    for (int i = 0; i < static_cast<int>(blocks.size()); i++)
    {
        if (blocks[i] == -1)
            continue;

        sum += i * blocks[i];
    }

    return std::to_string(sum);
}