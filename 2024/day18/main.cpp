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

std::vector<std::pair<int, int>> parseInput(std::stringstream &file_content)
{

    std::vector<std::pair<int, int>> result;
    std::string line;
    while (std::getline(file_content, line))
    {
        std::pair<int, int> p;
        std::string delimiter = ",";
        p.second = std::stoi(line.substr(0, line.find(delimiter)));
        p.first = std::stoi(line.substr(line.find(delimiter) + 1, line.length()));
        result.push_back(p);
    }
    return result;
}

int shortestPath(std::vector<std::string> &grid, int x, int y, int x_end, int y_end)
{
    std::vector<std::pair<int, int>> directions = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
    std::vector<std::tuple<int, int, int>> q;
    int width = x_end + 1;
    int height = y_end + 1;
    q.push_back({x, y, 0});

    while (!q.empty())
    {
        auto [x, y, steps] = q.front();
        q.erase(q.begin());
        if (x == x_end && y == y_end)
        {
            return steps;
        }
        for (auto [dx, dy] : directions)
        {
            int new_x = x + dx;
            int new_y = y + dy;
            if (new_x >= 0 && new_x < width && new_y >= 0 && new_y < height && grid[new_x][new_y] == '.')
            {
                grid[new_x][new_y] = '#';
                q.push_back({new_x, new_y, steps + 1});
            }
        }
    }

    return -1;
}

std::string part1(std::stringstream &file_content)
{

    std::vector<std::pair<int, int>> input = parseInput(file_content);
    std::vector<std::string> grid(71, std::string(71, '.'));

    for (size_t i = 0; i < 1024; i++)
    {
        if (i >= input.size())
        {
            break;
        }

        int x = input[i].first;
        int y = input[i].second;
        grid[x][y] = '#';
    }

    int steps = shortestPath(grid, 0, 0, 70, 70);

    return std::to_string(steps);
}

std::string part2(std::stringstream &file_content)
{

    std::vector<std::pair<int, int>> input = parseInput(file_content);
    std::vector<std::string> grid(71, std::string(71, '.'));

    for (size_t i = 0; i < 1024; i++)
    {
        if (i >= input.size())
        {
            break;
        }

        int x = input[i].first;
        int y = input[i].second;
        grid[x][y] = '#';
    }

    for (size_t i = 1024; i < input.size(); i++)
    {
        int x = input[i].first;
        int y = input[i].second;
        grid[x][y] = '#';
        auto cpy = grid;
        int steps = shortestPath(cpy, 0, 0, 70, 70);
        if (steps == -1)
        {
            return std::to_string(y) + "," + std::to_string(x);
        }
    }

    return "";
}
