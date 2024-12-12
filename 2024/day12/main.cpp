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

int bfs(std::vector<std::string> &grid, int i, int j)
{
    const std::array<std::pair<int, int>, 4> directions = {{{0, 1}, {0, -1}, {1, 0}, {-1, 0}}};
    std::vector<std::pair<int, int>> q;
    q.push_back({i, j});
    int cells = 0;
    int walls = 0;
    int width = grid[0].size();
    int height = grid.size();
    char s = grid[i][j];
    std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
    visited[i][j] = true;
    while (!q.empty())
    {
        auto [x, y] = q.back();
        q.pop_back();

        cells++;
        for (auto [dx, dy] : directions)
        {
            int nx = x + dx;
            int ny = y + dy;
            if (nx < 0 || nx >= height || ny < 0 || ny >= width || grid[nx][ny] != s)
            {
                walls++;
                continue;
            }
            if (visited[nx][ny])
                continue;
            visited[nx][ny] = true;
            q.push_back({nx, ny});
        }
    }

    for (size_t i = 0; i < visited.size(); i++)
    {
        for (size_t j = 0; j < visited[i].size(); j++)
        {
            if (visited[i][j])
            {
                grid[i][j] = ' ';
            }
        }
    }

    return walls * cells;
}

std::string part1(std::stringstream &file_content)
{

    std::vector<std::string> grid;
    for (std::string line; std::getline(file_content, line);)
    {
        grid.push_back(line);
    }

    int res = 0;

    for (size_t i = 0; i < grid.size(); i++)
    {
        for (size_t j = 0; j < grid[i].size(); j++)
        {
            if (grid[i][j] == ' ')
                continue;
            res += bfs(grid, i, j);
        }
    }

    return std::to_string(res);
}

int bfs2(std::vector<std::string> &grid, int i, int j)
{
    const std::array<int, 4> directions_int = {0, 1, 2, 3};
    const std::array<std::pair<int, int>, 4> directions = {{{0, 1}, {0, -1}, {1, 0}, {-1, 0}}};
    std::vector<std::pair<int, int>> q;
    q.push_back({i, j});
    int cells = 0;
    int width = grid[0].size();
    int height = grid.size();
    char s = grid[i][j];
    std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
    std::vector<std::vector<std::bitset<4>>> walls_visited(height, std::vector<std::bitset<4>>(width, 0));
    visited[i][j] = true;
    while (!q.empty())
    {
        auto [x, y] = q.back();
        q.pop_back();

        cells++;
        for (auto d : directions_int)
        {
            auto [dx, dy] = directions[d];
            int nx = x + dx;
            int ny = y + dy;
            if (nx < 0 || nx >= height || ny < 0 || ny >= width || grid[nx][ny] != s)
            {
                walls_visited[x][y][d] = true;
                continue;
            }
            if (visited[nx][ny])
                continue;
            visited[nx][ny] = true;
            q.push_back({nx, ny});
        }
    }

    for (size_t i = 0; i < visited.size(); i++)
    {
        for (size_t j = 0; j < visited[i].size(); j++)
        {
            if (visited[i][j])
            {
                grid[i][j] = ' ';
            }
        }
    }

    int wc=0;
    for (size_t i = 0; i < visited.size(); i++)
    {
        std::bitset<4> current = 0;
        for (size_t j = 0; j < visited[i].size(); j++)
        {
            if (!visited[i][j]){
                current = 0;
                continue;
            }

            if (!current[2] && walls_visited[i][j][2])
            {
                wc++;
            }
            if (!current[3] && walls_visited[i][j][3])
            {
                wc++;
            }

            current = walls_visited[i][j];
        }
    }

    for (size_t j = 0; j < visited[0].size(); j++)
    {
        std::bitset<4> current = 0;
        for (size_t i = 0; i < visited.size(); i++)
        {
            if (!visited[i][j]){
                current = 0;
                continue;
            }

            if (!current[0] && walls_visited[i][j][0])
            {
                wc++;
            }
            if (!current[1] && walls_visited[i][j][1])
            {
                wc++;
            }

            current = walls_visited[i][j];
        }
    }


    return cells*wc;
}

std::string part2(std::stringstream &file_content)
{

    std::vector<std::string> grid;
    for (std::string line; std::getline(file_content, line);)
    {
        grid.push_back(line);
    }

    int res = 0;

    for (size_t i = 0; i < grid.size(); i++)
    {
        for (size_t j = 0; j < grid[i].size(); j++)
        {
            if (grid[i][j] == ' ')
                continue;
            res += bfs2(grid, i, j);
        }
    }

    

    return std::to_string(res);
}
