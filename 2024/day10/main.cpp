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

int getTrailCount(const std::vector<std::string> &grid, int i, int j, bool checkVisited = true)
{

    int count{};
    std::vector<std::pair<int, int>> directions = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    std::vector<std::tuple<int, int>> queue = {{i, j}};
    std::bitset<10000> visited{};
    visited.set(i * 100 + j);
    int width = grid[0].size();
    int height = grid.size();

    while (!queue.empty())
    {
        auto [x, y] = queue.back();
        queue.pop_back();

        if (grid[x][y] == '9')
        {
            count++;
            continue;
        }

        for (auto [dx, dy] : directions)
        {
            int new_x = x + dx;
            int new_y = y + dy;

            if (new_x < 0 || new_x >= height || new_y < 0 || new_y >= width)
                continue;

            if (checkVisited && visited.test(new_x * 100 + new_y))
                continue;

            int diff = grid[new_x][new_y] - grid[x][y];

            if (diff != 1)
                continue;

            visited.set(new_x * 100 + new_y);
            queue.push_back({new_x, new_y});
        }
    }

    return count;
}

std::string part1(std::stringstream &file_content)
{

    std::vector<std::string> grid{};
    int trails_count{};

    for (std::string line; std::getline(file_content, line);)
        grid.push_back(line);

    int width = grid[0].size();
    int height = grid.size();

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (grid[i][j] != '0')
                continue;
            trails_count += getTrailCount(grid, i, j);
        }
    }

    return std::to_string(trails_count);
}

std::string part2(std::stringstream &file_content)
{

    std::vector<std::string> grid{};
    int trails_count{};

    for (std::string line; std::getline(file_content, line);)
        grid.push_back(line);

    int width = grid[0].size();
    int height = grid.size();

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (grid[i][j] != '0')
                continue;
            trails_count += getTrailCount(grid, i, j, false);
        }
    }

    return std::to_string(trails_count);
}
