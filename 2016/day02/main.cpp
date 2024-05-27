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

using key_pad_model = std::vector<std::vector<char>>;

std::array<key_pad_model, 2> key_pad_models = {
    {{{0, 0, 0, 0, 0},
      {0, '1', '2', '3', 0},
      {0, '4', '5', '6', 0},
      {0, '7', '8', '9', 0},
      {0, 0, 0, 0, 0}

     },
     {

         {0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, '1', 0, 0, 0},
         {0, 0, '2', '3', '4', 0, 0},
         {0, '5', '6', '7', '8', '9', 0},
         {0, 0, 'A', 'B', 'C', 0, 0},
         {0, 0, 0, 'D', 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0}}

    }};

std::string processFileContent(std::stringstream &file_content, const std::vector<std::vector<char>> &key_pad)
{
    std::string res{};
    int key_pad_size = key_pad.size();
    int start_x = 0;
    int start_y = 0;

    
    for (int i = 0; i < key_pad_size; i++)
    {
        bool found = false;
        for (int j = 0; j < key_pad_size; j++)
        {
            if (key_pad[i][j] == '5')
            {
                start_x = j;
                start_y = i;
                found = true;
                break;
            }
        }
        if (found)
        {
            break;
        }
    }

    for (std::string line; std::getline(file_content, line);)
    {
        for (char c : line)
        {
            int next_x = start_x;
            int next_y = start_y;
            switch (c)
            {
            case 'U':
                next_y--;
                break;
            case 'D':
                next_y++;
                break;
            case 'L':
                next_x--;
                break;
            case 'R':
                next_x++;
                break;
            }

            if (key_pad[next_y][next_x] != 0)
            {
                start_x = next_x;
                start_y = next_y;
            }
        }

        res += key_pad[start_y][start_x];
    }

    return res;
}

std::string part1(std::stringstream &file_content)
{
    return processFileContent(file_content, key_pad_models[0]);
}

std::string part2(std::stringstream &file_content)
{
    return processFileContent(file_content, key_pad_models[1]);
}
