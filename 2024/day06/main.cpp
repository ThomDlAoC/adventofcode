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
#include <bitset>

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

std::pair<int, int> getGuardLocation(std::vector<std::string> &grid)
{
    for (size_t i = 0; i < grid.size(); i++)
    {
        for (size_t j = 0; j < grid[i].size(); j++)
        {
            if (grid[i][j] == '^')
            {
                return {i, j};
            }
        }
    }
    return {-1, -1};
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

    std::pair<int, int> pos = getGuardLocation(grid);
    std::pair<int, int> dir = {-1, 0};

    grid[pos.first][pos.second] = 'X';

    auto oob = [&](const std::pair<int, int> &pos)
    {
        return pos.first < 0 || pos.first >= height || pos.second < 0 || pos.second >= width;
    };
    int cells = 1;
    while (!oob(pos))
    {
        int ny = pos.first + dir.first;
        int nx = pos.second + dir.second;

        if (oob({ny, nx}))
        {
            break;
        }

        if (grid[ny][nx] == '#')
            dir = {dir.second, -dir.first};
        else
            pos = {ny, nx};

        if (grid[pos.first][pos.second] == '.')
        {
            grid[pos.first][pos.second] = 'X';
            cells++;
        }
    }

    return std::to_string(cells);
}

#define DIR_UP 0
#define DIR_RIGHT 1
#define DIR_DOWN 2
#define DIR_LEFT 3

std::vector<std::pair<int, int>> dirs = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
std::vector<std::bitset<4>> dirs_bit = {0b0001, 0b0010, 0b0100, 0b1000};

bool hasLoop(const std::vector<std::string> &grid, std::pair<int, int> pos, int height, int width, int mi, int mj)
{

    std::vector<std::vector<std::bitset<4>>> grid_bits(height, std::vector<std::bitset<4>>(width, 0));

    grid_bits[pos.first][pos.second] = dirs_bit[DIR_UP];
    int current_dir = DIR_UP;
    std::pair<int, int> dir = dirs[current_dir];

    auto oob = [&](const std::pair<int, int> &pos)
    {
        return pos.first < 0 || pos.first >= height || pos.second < 0 || pos.second >= width;
    };

    while (!oob(pos))
    {
        int ny = pos.first + dir.first;
        int nx = pos.second + dir.second;

        if (oob({ny, nx}))
            return false;

        if (grid[ny][nx] == '#' || (ny == mi && nx == mj))
        {
            current_dir = (current_dir + 1) % 4;
            dir = dirs[current_dir];
        }
        else
            pos = {ny, nx};

        if (grid_bits[pos.first][pos.second].test(current_dir))
        {
            return true;
        }

        grid_bits[pos.first][pos.second] |= dirs_bit[current_dir];
    }

    return false;
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

    std::pair<int, int> pos = getGuardLocation(grid);
    auto initial_pos = pos;
    std::vector<std::pair<int, int>> dirs = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
    std::vector<std::bitset<4>> dirs_bit = {0b0001, 0b0010, 0b0100, 0b1000};

    std::vector<std::vector<std::bitset<4>>> grid_bits(height, std::vector<std::bitset<4>>(width, 0));

    grid[pos.first][pos.second] = 'X';
    grid_bits[pos.first][pos.second] = dirs_bit[DIR_UP];
    int current_dir = DIR_UP;
    std::pair<int, int> dir = dirs[current_dir];

    auto oob = [&](const std::pair<int, int> &pos)
    {
        return pos.first < 0 || pos.first >= height || pos.second < 0 || pos.second >= width;
    };

    while (!oob(pos))
    {
        int ny = pos.first + dir.first;
        int nx = pos.second + dir.second;

        if (oob({ny, nx}))
            break;

        if (grid[ny][nx] == '#')
        {
            current_dir = (current_dir + 1) % 4;
            dir = dirs[current_dir];
        }
        else
            pos = {ny, nx};
        grid_bits[pos.first][pos.second] |= dirs_bit[current_dir];
    }

    int places = 0;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (grid[i][j] == '#')
                continue;
            if (grid_bits[i][j].count() == 0)
                continue;
            places += hasLoop(grid, initial_pos, height, width, i, j);
        }
    }

    return std::to_string(places);
}
