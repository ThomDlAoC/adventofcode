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
#include <unordered_set>

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

std::pair<std::pair<int, int>, std::pair<int, int>> getLocations(const std::vector<std::string> &grid)
{
    std::pair<int, int> start, end;
    for (size_t i = 0; i < grid.size(); ++i)
    {
        for (size_t j = 0; j < grid[i].size(); ++j)
        {
            if (grid[i][j] == 'S')
            {
                start = {i, j};
            }
            else if (grid[i][j] == 'E')
            {
                end = {i, j};
            }
        }
    }
    return {start, end};
}

int cheatCountSavingTime(const std::vector<std::string> &grid, std::pair<int, int> start, std::pair<int, int> end, int cheatThreshold, int cl)
{
    int width = grid[0].size();
    int height = grid.size();

    auto isOOb = [&](int x, int y)
    {
        return x < 0 || y < 0 || x >= width || y >= height;
    };

    auto isWall = [&grid](int x, int y)
    {
        return grid[y][x] == '#';
    };

    std::vector<std::vector<int>> dist(height, std::vector<int>(width, INT_MAX));
    dist[start.first][start.second] = 0;

    std::vector<std::pair<int, int>> directions = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

    std::vector<std::pair<int, int>> q = {start};

    for (size_t i = 0; i < q.size(); ++i)
    {
        auto [y, x] = q[i];
        for (auto [dx, dy] : directions)
        {
            int nx = x + dx;
            int ny = y + dy;
            if (isOOb(nx, ny) || isWall(nx, ny))
            {
                continue;
            }

            if (dist[ny][nx] > dist[y][x] + 1)
            {
                dist[ny][nx] = dist[y][x] + 1;
                q.push_back({ny, nx});
            }
        }
    }

    int count = 0;

    // for each cell that is not a wall
    // check all cells in a range of cl

    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            if (grid[i][j] == '#')
            {
                continue;
            }

            for (int k = -cl; k <= cl; ++k)
            {
                for (int l = -cl; l <= cl; ++l)
                {
                    if (k == 0 && l == 0)
                    {
                        continue;
                    }

                    int nx = j + l;
                    int ny = i + k;

                    if (isOOb(nx, ny) || isWall(nx, ny))
                    {
                        continue;
                    }

                    int dd = std::abs(k) + std::abs(l);
                    if (dd > cl)
                    {
                        continue;
                    }

                    int diff = dist[ny][nx] - dist[i][j] - dd;
                    if (diff < cheatThreshold)
                    {
                        continue;
                    }

                    count++;
                }
            }
        }
    }

    return count;
}

std::string part1(std::stringstream &file_content)
{

    std::vector<std::string> grid;
    std::string line;
    while (std::getline(file_content, line))
    {
        grid.push_back(line);
    }

    auto [S, E] = getLocations(grid);

    return std::to_string(cheatCountSavingTime(grid, S, E, 100, 2));
}

std::string part2(std::stringstream &file_content)
{

    std::vector<std::string> grid;
    std::string line;
    while (std::getline(file_content, line))
    {
        grid.push_back(line);
    }

    auto [S, E] = getLocations(grid);

    return std::to_string(cheatCountSavingTime(grid, S, E, 100, 20));

    return "";
}
