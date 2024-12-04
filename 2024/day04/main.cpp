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
#include <array>

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

bool searchDir(const std::vector<std::string> &grid, int x, int y, int width, int height, std::pair<int, int> dir)
{

    const std::string str_search = "XMAS";
    for (int i = 0; i < 4; i++)
    {
        int new_x = x + i * dir.first;
        int new_y = y + i * dir.second;
        if (new_x < 0 || new_x >= width || new_y < 0 || new_y >= height)
        {
            return false;
        }
        if (grid[new_y][new_x] != str_search[i])
        {
            return false;
        }
    }
    return true;
}


std::string part1(std::stringstream &file_content)
{
    std::vector<std::string> grid;
    for (std::string line; std::getline(file_content, line);)
    {
        grid.push_back(line);
    }

    int width = grid[0].size();
    int height = grid.size();

    const std::array<std::pair<int, int>, 8> dirs = {
        {{1, 0}, {0, 1}, {1, 1}, {1, -1}, {-1, 0}, {0, -1}, {-1, -1}, {-1, 1}}};

    int total_xmas{};
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            for (const auto &dir : dirs)
            {
                total_xmas += searchDir(grid, x, y, width, height, dir);
            }
        }
    }

    return std::to_string(total_xmas);
}

std::string part2(std::stringstream &file_content)
{

    std::vector<std::string> grid;
    for (std::string line; std::getline(file_content, line);)
    {
        grid.push_back(line);
    }

    int width = grid[0].size();
    int height = grid.size();

    int total_xmas{};
    for (int y = 1; y < height - 1; y++)
    {
        for (int x = 1; x < width - 1; x++)
        {
            if (grid[y][x] != 'A')
                continue;

            total_xmas += ((('M' ^ 'S') == (grid[y - 1][x - 1] ^ grid[y + 1][x + 1])) && (('M' ^ 'S') == (grid[y - 1][x + 1] ^ grid[y + 1][x - 1])));
        }
    }

    return std::to_string(total_xmas);
}

